const screenshot = require('./build/Release/screenshot.node')
const {
    performance
} = require('perf_hooks')

console.log(screenshot.hello())
let s = performance.now()
// for (let i = 0; i < 24; i++) {
//     screenshot.getBitmap()
// }
// console.log(performance.now() - s)
let buf = screenshot.getBitmap()
// setInterval(() => {
    // let s = performance.now()
    let buf1 = screenshot.getBitmap()
    // console.log(performance.now() - s)
// }, 1000 / 24)
let buf2 = Buffer.alloc(buf1.length);
for (let i = 0; i < buf.length; i++) {
    buf2[i] = buf1[i] - buf[i]
}
console.log(performance.now() - s)
console.log(buf2)