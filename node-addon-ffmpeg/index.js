const ffmpeg = require('./build/Release/ffmpeg.node')
console.log(ffmpeg.init("D:\\Wildlife.wmv"))
console.log(ffmpeg.getInfo())
console.log(ffmpeg.readPacket(function(){}))
setTimeout(function() {

    console.log(ffmpeg.decodeVideo())
    console.log(ffmpeg.decodeVideo())
    console.log(ffmpeg.decodeVideo())
    console.log(ffmpeg.decodeVideo())
}, 1000)

// console.log(ffmpeg.config("D:\\Wildlife.wmv"))
// console.log(ffmpeg.extractRGBFrame())

// console.log(ffmpeg.initAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())