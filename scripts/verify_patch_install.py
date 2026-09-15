#!/usr/bin/env python3
"""Compare an installed tracked tree with a patch applied to a pinned base."""

from __future__ import annotations

import argparse
import ast
import re
import stat
import subprocess
import tempfile
from pathlib import Path, PurePosixPath


def run(*args: str, cwd: Path, check: bool = True) -> subprocess.CompletedProcess[bytes]:
    return subprocess.run(args, cwd=cwd, check=check, stdout=subprocess.PIPE, stderr=subprocess.PIPE)


def safe_patch_paths(patch: Path) -> dict[str, str]:
    paths: dict[str, str] = {}
    lines = patch.read_text(encoding="utf-8").splitlines()
    for index, line in enumerate(lines):
        match = re.match(r"^diff --git a/(\S+) b/(\S+)$", line)
        if not match:
            continue
        old, new = match.groups()
        if old != new:
            raise ValueError(f"renames are not supported by the artifact patch: {old} -> {new}")
        pure = PurePosixPath(new)
        if pure.is_absolute() or ".." in pure.parts:
            raise ValueError(f"unsafe patch path: {new}")
        end = next((j for j in range(index + 1, len(lines)) if lines[j].startswith("diff --git ")), len(lines))
        section = lines[index:end]
        if any(value.startswith("new file mode") for value in section):
            paths[new] = "added"
        elif any(value.startswith("deleted file mode") for value in section):
            paths[new] = "deleted"
        else:
            paths[new] = "modified"
    if not paths:
        raise ValueError(f"patch contains no file diffs: {patch}")
    return paths


def baseline_file(repo: Path, commit: str, name: str, target: Path) -> None:
    shown = run("git", "show", f"{commit}:{name}", cwd=repo, check=False)
    if shown.returncode != 0:
        return
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_bytes(shown.stdout)
    mode_text = run("git", "ls-tree", commit, "--", name, cwd=repo).stdout.decode()
    mode = int(mode_text.split(maxsplit=1)[0], 8)
    target.chmod(0o755 if mode & 0o111 else 0o644)


def compare_patch(repo: Path, commit: str, patch: Path) -> list[str]:
    patch_status = safe_patch_paths(patch)
    patch_paths = sorted(patch_status)
    actual = run("git", "diff", "--name-only", "-z", commit, "--", cwd=repo).stdout
    actual_paths = [value.decode() for value in actual.split(b"\0") if value]
    untracked = run("git", "ls-files", "--others", "--exclude-standard", "-z", cwd=repo).stdout
    untracked_paths = [value.decode() for value in untracked.split(b"\0") if value]
    actual_relevant = set(actual_paths)
    for name, status in patch_status.items():
        if status == "added" and name in untracked_paths:
            actual_relevant.add(name)
    missing = sorted(set(patch_paths) - actual_relevant)
    extra = sorted(set(actual_paths) - set(patch_paths))
    if missing or extra:
        raise ValueError(f"tracked diff path mismatch; missing={missing}, extra={extra}")

    with tempfile.TemporaryDirectory(prefix="hbf-patch-check-") as directory:
        expected_root = Path(directory)
        for name in patch_paths:
            baseline_file(repo, commit, name, expected_root / name)
        applied = run(
            "git", "apply", "--whitespace=nowarn", str(patch.resolve()),
            cwd=expected_root, check=False,
        )
        if applied.returncode != 0:
            raise ValueError("active patch does not apply to the pinned baseline: " + applied.stderr.decode().strip())
        for name in patch_paths:
            expected = expected_root / name
            installed = repo / name
            if expected.exists() != installed.exists():
                raise ValueError(f"installed path existence differs from active patch: {name}")
            if not expected.exists():
                continue
            if expected.read_bytes() != installed.read_bytes():
                raise ValueError(f"installed tracked content differs from active patch: {name}")
            expected_exec = bool(expected.stat().st_mode & stat.S_IXUSR)
            installed_exec = bool(installed.stat().st_mode & stat.S_IXUSR)
            if expected_exec != installed_exec:
                raise ValueError(f"installed executable mode differs from active patch: {name}")
    return patch_paths


def read_manifest(path: Path) -> list[str]:
    result: list[str] = []
    for raw in path.read_text(encoding="utf-8").splitlines():
        value = raw.split("#", 1)[0].strip()
        if value:
            result.append(value)
    if not result or len(result) != len(set(result)):
        raise ValueError("source manifest must be nonempty and unique")
    return result


def check_options(config: Path, gpu_sim: Path) -> int:
    options = re.findall(r"^\s*(-gpgpu_hbf_[A-Za-z0-9_]+)(?:\s|$)", config.read_text(encoding="utf-8"), re.MULTILINE)
    if not options or len(options) != len(set(options)):
        raise ValueError("canonical config HBF options must be nonempty and unique")
    source = gpu_sim.read_text(encoding="utf-8")
    missing = [
        option for option in options
        if not re.search(r'option_parser_register\s*\(\s*opp\s*,\s*"' + re.escape(option) + r'"', source)
    ]
    if missing:
        raise ValueError("canonical config options lack parser registration: " + ", ".join(missing))
    return len(options)


def check_cmake(manifest: list[str], cmake: Path) -> int:
    sources = [name for name in manifest if name.endswith(".cc")]
    content = re.sub(r"#[^\n]*", "", cmake.read_text(encoding="utf-8"))
    missing = [name for name in sources if not re.search(r"(?<![A-Za-z0-9_.+-])" + re.escape(name) + r"(?![A-Za-z0-9_.+-])", content)]
    if missing:
        raise ValueError("manifest sources absent from CMake: " + ", ".join(missing))
    return len(sources)


def check_trace_schema(trace_source: Path) -> None:
    source = trace_source.read_text(encoding="utf-8")
    match = re.search(r'fprintf\s*\(\s*m_file\s*,\s*((?:"(?:\\.|[^"\\])*"\s*)+)\);', source, re.DOTALL)
    if not match:
        raise ValueError("could not locate trace header writer")
    header = "".join(ast.literal_eval(token) for token in re.findall(r'"(?:\\.|[^"\\])*"', match.group(1)))
    expected = (
        "sim_cycle,request_id,source_subpartition,op,address,page,channel,"
        "subarray,state,bytes,queue_depth,latency,cache_hit,mshr_hit,error\n"
    )
    if header != expected:
        raise ValueError(f"trace writer schema drifted from hbf-trace-v1: {header!r}")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, required=True)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--patch", type=Path, required=True)
    parser.add_argument("--manifest", type=Path, required=True)
    parser.add_argument("--config", type=Path, required=True)
    args = parser.parse_args()

    manifest = read_manifest(args.manifest)
    patch_paths = compare_patch(args.repo, args.commit, args.patch)
    registered = check_options(args.config, args.repo / "src/gpgpu-sim/gpu-sim.cc")
    compiled = check_cmake(manifest, args.repo / "src/gpgpu-sim/CMakeLists.txt")
    check_trace_schema(args.repo / "src/gpgpu-sim/hbf_trace.cc")
    print(f"artifact-check: {len(patch_paths)} tracked paths exactly match active patch")
    print(f"artifact-check: {registered} canonical HBF options are registered")
    print(f"artifact-check: {compiled} manifest C++ sources are in CMake")
    print("artifact-check: trace writer schema is exact hbf-trace-v1")
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, subprocess.SubprocessError, ValueError) as exc:
        raise SystemExit(f"artifact-check: {exc}")
