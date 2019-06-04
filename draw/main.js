const dgram = require('dgram');
const server = dgram.createSocket('udp4');
const zlib = require('zlib')

let ctx = canvas.getContext('2d')
let imgData = ctx.createImageData(1600, 900)

function draw(buf) {
    if (!buf) return;
    // buf = zlib.gunzipSync(buf)
    console.log('buf', buf)
    for (let i = 0, j = 0; i < buf.length; i += 3, j += 4) {
        imgData.data[j] = buf[i + 2]
        imgData.data[j + 1] = buf[i + 1]
        imgData.data[j + 2] = buf[i]
        imgData.data[j + 3] = 255//buf[i + 3]
    }
    console.log('imgdata', imgData.data)
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
    let isEnd = msg.readUInt8(4)
    let data = msg.slice(5)
    // console.log(num, isEnd)
    if (!pixels[num]) {
        pixels[num] = Buffer.alloc(0);
    }
    pixels[num] = Buffer.concat([pixels[num], data], pixels[num].length + data.length)

    if (isEnd) {
        // if (pixels[num]) {
            // let len = 0
            // for (let b of pixels[num]) {
            //     len += b.length
            // }
            // let buf = Buffer.concat(pixels[num], len)
            // console.log(len)
            // draw(buf)
        // }
        // console.log(pixels[num])
        draw(pixels[num])
        delete pixels[num]
    }
    
});

server.on('listening', () => {
  const address = server.address();
  console.log(`server listening ${address.address}:${address.port}`);
});

server.bind(41234);