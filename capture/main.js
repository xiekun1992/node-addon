const screenshot = require('../node-addon-screenshot/build/Release/screenshot.node')
const zlib = require('zlib')
const {
    performance
} = require('perf_hooks')
const dgram = require('dgram');
let server = dgram.createSocket('udp4');

let buf = Buffer.from(screenshot.getBitmap())
let buf2 = Buffer.alloc(buf.length);

let count = 0;
zlib.gzip(buf,  (err, res) => {
    seqPacket(res)
    cap();
});

function cap() {
    setInterval(() => {
        let buf1 = Buffer.from(screenshot.getBitmap())
        for (let i = 0; i < buf.length; i++) {
            buf2[i] = buf1[i] - buf[i]
        }
        buf = buf1;
        // console.log(buf2)
        zlib.gzip(buf2,  (err, res) => {
            seqPacket(res)
        });
        // seqPacket(Buffer.alloc(1, 1))
    }, 1000 / 24)
}

function seqPacket(buf) {
    // return buf;
    sendPacket(buf, count)
    count++
}
// https://nodejs.org/dist/latest-v8.x/docs/api/dgram.html#dgram_socket_send_msg_offset_length_port_address_callback
let mtu = 65507
function sendPacket(buf, num) {
    if (buf.length > 0) {
        let numbuf = Buffer.alloc(4)
        numbuf.writeInt32BE(num)
        let isEnd = Buffer.alloc(1)
        isEnd.writeUInt8(1)

        if (buf.length + isEnd.length + buf.length > mtu) {
            isEnd = Buffer.alloc(1)
            isEnd.writeUInt8(0)
        }
        buf = Buffer.concat([numbuf, isEnd, buf], numbuf.length + isEnd.length + buf.length)
        tmpbuf = buf.slice(0, mtu);
        server.send(tmpbuf, 41234, 'localhost', (err, bytes) => {
            err && console.log(err, bytes)
        });
        sendPacket(buf.slice(mtu), num);
    }
}