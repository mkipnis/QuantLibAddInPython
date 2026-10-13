# QuantLibAddInPython

```
python3 -m venv .venv
source .venv/bin/activate

python -m pip install --upgrade pip
python -m pip install --upgrade build scikit-build-core pybind11 ninja

python -m build --wheel --no-isolation --verbose
python -m pip install dist/*.whl --force-reinstall
```
