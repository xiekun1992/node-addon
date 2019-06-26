const ffmpeg = require('./build/Release/ffmpeg.node')

// console.log(ffmpeg.config("D:\\Wildlife.wmv"))
// console.log(ffmpeg.extractRGBFrame())

console.log(ffmpeg.initAudio())
console.log(ffmpeg.decodeAudio())
console.log(ffmpeg.decodeAudio())
console.log(ffmpeg.decodeAudio())
console.log(ffmpeg.decodeAudio())