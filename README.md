  eve -- an elegant game dev framework
============================================================================

Welcome to *eve*, an elegant and simple framework for fun and quick game development.

Getting started
---------------

1. Checkout the code repo from GitHub.

2. Download the extern dependencies zip file from [here)(https://dl.dropboxusercontent.com/u/1317535/files/extern.zip).

* Note: one day all extern dependencies will be put in a separate repository.

2. Extract the content of the zip into the project root (`extern` folder should be a sibling of `include` and `src`).

3. Open a prompt in the root folder and run
  
  ```bash
  extern/bin/premake4.exe vs2012
  ```

  to generate the Visual Studio 2012 (November CTP) solution. Type "gmake"
  instead of "vs2012" to generate a GNU makefile instead.

4. Go to the [wiki](https://github.com/keebus/eve/wiki). You'll find further getting started info there.

