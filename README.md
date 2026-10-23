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

$env:CMAKE_ARGS = "-DBOOST_ROOT=C:/boost -DBoost_ROOT=C:/boost -DBoost_INCLUDE_DIR=C:/boost/include -DBoost_LIBRARY_DIR_RELEASE=C:/boost/lib"

python -m build --wheel --no-isolation --verbose
 python -m pip install .\dist\quantlib_addin-0.1.0-cp313-cp313-win_amd64.whl --force-reinstall
```

#### Windows boost/b2 setup:
```
.\b2 --with-filesystem --with-system --with-serialization --with-date_time --with-chrono --with-json --with-atomic --with-container architecture=x86 address-model=64 variant=release link=static runtime-link=shared threading=multi --layout=system --prefix=C:\boost install
```

### To Regenerate Bindings
export PYTHONPATH=$PYTHONPATH:pybind_generator
python pybind_generator/generate_pybind_bindings.py ./build/cp312-cp312-linux_x86_64/_deps/quantlibaddin_repo-src/QuantLibAddin/Addins/Cpp bindings/QuantLibAddinCpp.cpp --object-handler-function-dir ./build/cp312-cp312-linux_x86_64/_deps/quantlibaddin_repo-src/ObjectHandler/gensrc/metadata/functions/ --quantlib-addin-function-dir ./build/cp312-cp312-linux_x86_64/_deps/quantlibaddin_repo-src/QuantLibAddin/gensrc/metadata/functions/
