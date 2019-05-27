const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const zlib = require('zlib')

let ctx = canvas.getContext('2d')
let imgData = ctx.createImageData(1600, 900)

function draw(buf) {
    if (!buf) return;
    buf = zlib.gunzipSync(buf)
    for (let i = 0; i < buf.length; i += 4) {
        imgData.data[i] += buf[i + 2]
        imgData.data[i + 1] += buf[i + 1]
        imgData.data[i + 2] += buf[i]
        imgData.data[i + 3] += buf[i + 3]
    }
    ctx.putImageData(imgData, 0, 0);
}

let pixels = {}
let current = -1

server.on('error', (err) => {
  console.log(`server error:\n${err.stack}`);
  server.close();
});
server.on('message', (msg, rinfo) => {
//   console.log(`server got: ${msg} from ${rinfo.address}:${rinfo.port}`);
    let num = msg.readInt32BE(0)
    let isEnd = msg.writeUInt8(4)
    console.log(num, isEnd)
    if (!pixels[num]) {
        pixels[num] = [];
    }
    pixels[num].push(msg.slice(5))

    if (isEnd) {
        if (pixels[num]) {
            let len = 0
            for (let b of pixels[num]) {
                len += b.length
            }
            let buf = Buffer.concat(pixels[num], len)
            draw(buf)
        }
        delete pixels[num]
    }
    
});

server.on('listening', () => {
  const address = server.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

server.bind(41234);