const ffmpeg = require('./build/Release/ffmpeg.node')
console.log(ffmpeg.init("D:\\Wildlife.wmv"))
// console.log(ffmpeg.getInfo())
console.log(ffmpeg.readPacket(function(){}))
setTimeout(() => {
    console.log(ffmpeg.update((arg, arg1) => {
        console.log("JavaScript callback called with arguments", arg, arg1)
    }, 5))
}, 1000)
// setInterval(function() {
//     console.log('===', ffmpeg.decodeVideo())
// }, 10)

// console.log(ffmpeg.config("D:\\Wildlife.wmv"))
// console.log(ffmpeg.extractRGBFrame())

// console.log(ffmpeg.initAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())