const screenshot = require('./build/Release/screenshot.node')
const {
    performance
} = require('perf_hooks')

console.log(screenshot.hello())
// let s = performance.now()
// for (let i = 0; i < 24; i++) {
//     screenshot.getBitmap()
// }
// console.log(performance.now() - s)
let buf = Buffer.from(screenshot.getBitmap())
console.log(buf.length)
let buf2 = Buffer.alloc(buf.length);
console.log(buf2.length)
setInterval(() => {
    // let s = performance.now()
    let buf1 = Buffer.from(screenshot.getBitmap())
    console.log(buf1.length)
    // console.log(performance.now() - s)
    for (let i = 0; i < buf.length; i++) {
        buf2[i] = buf1[i] - buf[i]
    }
    buf = buf1;
}, 1000 / 24)
// console.log(performance.now() - s)
// console.log(buf2)