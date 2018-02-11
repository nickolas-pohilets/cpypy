# cpypy
Demo of debugging hybrid C++/Python project using two debuggers (Xcode and PyCharm).

It uses [remote debugging](https://www.jetbrains.com/help/pycharm/remote-debugging.html) feature, which is supported only in Profession Edition of PyCharm.

## Installation

You'll need PyCharm Profession Edition, Python 3.6, NumPy and Boost.
I've been using Xcode as C++ debugger, but any other C++ debugger/IDE should work as well.

I've used [MacPorts](https://www.macports.org) for installing Python, NumPy and Boost:
```bash
sudo port install python36
sudo port install py36-numpy
sudo port install boost +no_single +no_static +python36
``` 
**Note**

When installing `Boost` with `MacPorts`, `Boost.Python.NumPy` won't be installed unless `NumPy` was previously installed using `MacPorts`.

## Running

1. Open project in PyCharm and create a run/debug configuration for remote debugging pointing to `localhost:5678`.
1. Start PyCharm's debugger first. 
1. Open project in Xcode and run it.
1. C++ app will get stuck when starting `pydevd` on [this line](https://github.com/nickolas-pohilets/cpypy/blob/master/cpypy/main.cpp#L71) and PyCharm debugger will get activated.
1. Put a breakpoint in a loop in [fibonacci.py](https://github.com/nickolas-pohilets/cpypy/blob/master/cpypy/fibonacci.py#L6) and continue execution in PyCharm.
1. In PyCharm's debugger set value of `stop` to `True` to exit from the loop.
1. Unfortunately, PyCharm does not allow to set a Python breakpoint in C++ code, but if you leave from the last Python fragment using **Step Over**, then Python debugger will get activated when you enter next Python fragment.
1. Continue running the code, and it will create a tiny 2x3 gray scale image in `~/foo.png`.
