const ffmpeg = require('./build/Release/ffmpeg.node')

console.log(ffmpeg.config())
console.log(ffmpeg.extractRGBFrame())