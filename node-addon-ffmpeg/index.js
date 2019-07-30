const ffmpeg = require('./build/Release/ffmpeg.node')
console.log(ffmpeg.init("D:\\Wildlife.wmv"))
console.log(ffmpeg.getInfo())
console.log(ffmpeg.readPacket(function(){}))
setInterval(function() {
    console.log(ffmpeg.decodeVideo())
}, 100)

// console.log(ffmpeg.config("D:\\Wildlife.wmv"))
// console.log(ffmpeg.extractRGBFrame())

// console.log(ffmpeg.initAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())
// console.log(ffmpeg.decodeAudio())