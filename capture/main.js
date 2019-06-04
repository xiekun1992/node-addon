const screenshot = require('../node-addon-screenshot/build/Release/screenshot.node')
const zlib = require('zlib')
const {
    performance
} = require('perf_hooks')
const dgram = require('dgram');
let server = dgram.createSocket('udp4');

let buf = Buffer.from(screenshot.getBitmap())
// let buf2 = Buffer.alloc(buf.length);

let mtu = 65507
let count = 1;
// let res = zlib.gzipSync(buf);
// console.log(res.length)
// seqPacket(res)
// cap();

function cap() {
    let timer = setInterval(async () => {
        let buf2 = Buffer.from(screenshot.getBitmap())
        // for (let i = 0; i < buf.length; i++) {
        //     buf2[i] = buf1[i] - buf[i]
        // }
        // buf = buf1;
        // console.log('buf1', buf1)
        // console.log('buf2', buf2)
        // let res = zlib.gzipSync(buf2)
        await seqPacket(buf2)
        // seqPacket(Buffer.alloc(1, 1))
        // clearInterval(timer)
    }, 1000 / 24)
}

async function seqPacket(buf) {
    // return buf;
    await sendPacket(buf, count)
    count++
}
// https://nodejs.org/dist/latest-v8.x/docs/api/dgram.html#dgram_socket_send_msg_offset_length_port_address_callback
async function sendPacket(buf, num) {
    if (buf.length > 0) {
        let numbuf = Buffer.alloc(4)
        numbuf.writeInt32BE(num)
        let isEnd = Buffer.alloc(1)
        isEnd.writeUInt8(1)

        if (buf.length + isEnd.length + numbuf.length > mtu) {
            isEnd = Buffer.alloc(1)
            isEnd.writeUInt8(0)
        }
        tmpbuf = Buffer.concat([numbuf, isEnd, buf], numbuf.length + isEnd.length + buf.length)
        await send(tmpbuf);
        sendPacket(tmpbuf.slice(mtu), num);
    }
}

async function send(tmpbuf) {
    server.send(tmpbuf.slice(0, mtu), 41234, 'localhost', (err, bytes) => {
        err && console.log(err, bytes)
        Promise.resolve(true)
    });
}