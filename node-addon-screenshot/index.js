const screenshot = require('./build/Release/screenshot.node')
const zlib = require('zlib')
const {
    performance
} = require('perf_hooks')

console.log(screenshot.hello())
let s = performance.now()
// for (let i = 0; i < 24; i++) {
//     screenshot.getBitmap()
// }
// console.log(performance.now() - s)
// let buf = Buffer.from(screenshot.getBitmap())
// console.log(buf.length)
// let buf2 = Buffer.alloc(buf.length);
// console.log(buf2.length)
// // setTimeout(() => {

//     setInterval(() => {
//         // let s = performance.now()
//         let buf1 = Buffer.from(screenshot.getBitmap())
//         // console.log(buf1.length)
//         // console.log(performance.now() - s)
//         for (let i = 0; i < buf.length; i++) {
//             buf2[i] = buf1[i] - buf[i]
//         }
//         buf = buf1;
//         // console.log(buf2)
//         zlib.gzip(buf2,  (err, res) => {
//             console.log(res.length)
//         });
//     }, 1000 / 24)
// // }, 3000)
// // console.log(performance.now() - s)
// // console.log(buf2)

const sharp = require('sharp')

let buf = Buffer.from(screenshot.getBitmap())
let jpg = sharp(buf.slice(0, 1600 * 3), {
    raw: {
        width: 1600,
        height: 1,
        // height: 900,
        channels: 3
    }
}).jpeg().toBuffer().then(d => {
    console.log(performance.now() - s)
    console.log(d.length)
})
// console.log(jpg)