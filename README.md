# LTM
Chương trình mô phỏng IOT Gateway và IOT Node bằng Websocket qua boost và qt6.

Iot Node (ltm_client) sẽ gửi cho Iot Gateway (ltm_server) cặp tọa độ (x, y). Gateway sẽ hiển thị điểm trên đồ thị. Người dùng có thể Pause, Quit, các Node.




## Authors

- Kiều Minh Tuấn: 21020394 [@nam7v3](https://www.github.com/nam7v3)
- Đỗ Minh Tuấn: 21020032 [@Entropt](https://www.github.com/Entropt)
- Hoàng Minh Thái: 21020034 [@Kujoh13](https://github.com/Kujoh13)


## Installation

### Dependencies
- qt6
- boost: beast

### Install
``` console
$ cd ltm
$ mkdir build
$ cmake -S . -B build
$ cmake --build build
```
## Demo

![alt text](https://imgur.com/fz11wvz.png)
## Usage

```console
$ ltm_client <ip> <port> <sec delay between data>
$ ltm_server
```

