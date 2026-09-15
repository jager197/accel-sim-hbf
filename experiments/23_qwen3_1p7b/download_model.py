from pathlib import Path
import json
from huggingface_hub import snapshot_download
HERE=Path(__file__).resolve().parent
MODEL='Qwen/Qwen3-1.7B'
REVISION='70d244cc86ccca08cf5af4e1e306ecf908b1ad5e'
path=snapshot_download(MODEL,revision=REVISION,local_dir=HERE/'model',allow_patterns=['*.json','*.safetensors','*.txt','*.jinja','README.md'],max_workers=4)
(HERE/'model_source.json').write_text(json.dumps(dict(model=MODEL,revision=REVISION,path=path),indent=2)+'\n')
