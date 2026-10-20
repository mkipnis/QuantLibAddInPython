## QuantLibAddInPython

### Linux/Mac
```
python3 -m venv .venv
source .venv/bin/activate

python -m pip install --upgrade pip
python -m pip install --upgrade build scikit-build-core pybind11 ninja

python -m build --wheel --no-isolation --verbose
python -m pip install dist/*.whl --force-reinstall
```

### Windows
```
py -m venv .venv
.venv\Scripts\activate.bat

python -m pip install --upgrade pip
python -m pip install --upgrade build scikit-build-core pybind11 ninja

$env:CMAKE_ARGS = "-DBOOST_ROOT=C:/boost -DBoost_ROOT=C:/boost -DBoost_INCLUDE_DIR=C:/boost/include -DBoost_LIBRARY_DIR_RELEASE=C:/boost/lib -DBoost_USE_STATIC_RUNTIME=ON"

python -m build --wheel --no-isolation --verbose

python -m build --wheel --no-isolation --verbose
```