# webserver
A tiny webserver written in C for Linux

## compile:
```
mkdir builddir &&
meson setup builddir &&
cp index.html builddir &&
cd builddir &&
meson compile
```

## Run the server:
sudo ./webserver
