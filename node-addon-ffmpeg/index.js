const ffmpeg = require('./build/Release/ffmpeg.node');
const EventEmitter = require('events');

const event = new EventEmitter();

let info, videoInitialized = false

function play(filename) {
  console.log(filename);  // Prints "whoooooooh!"
  ffmpeg.init(filename)
  info = ffmpeg.getInfo()
  if (!videoInitialized) {
    videoInitialized = true
    if (info.video) {
      var scene = new THREE.Scene();
      var camera = new THREE.PerspectiveCamera( 45, window.innerWidth/window.innerHeight, 0.1, 1000 );
      
      var renderer = new THREE.WebGLRenderer();
      renderer.setSize( window.innerWidth, window.innerHeight );
      document.querySelector('#playerContainer').appendChild( renderer.domElement );
      window.addEventListener( 'resize', onWindowResize, false );
      
      function onWindowResize(){
        camera.aspect = window.innerWidth / window.innerHeight;
        camera.updateProjectionMatrix();
        renderer.setSize( window.innerWidth, window.innerHeight );
      }
              
      var width = +info.video.width, height = +info.video.height, interval = Math.ceil(1000 / +info.video.fps);
      var ratio = width / height
      // var data = ffmpeg.decodeVideo()
      var data = new Uint8Array(width * height * 3)
      
      var texture = new THREE.DataTexture( data, width, height, THREE.RGBFormat );
      var geometry = new THREE.PlaneGeometry( 27 * ratio, 27, 1, 1 )
      // var geometry = new THREE.PlaneGeometry( 48, 27, 1, 1 )
      // var material = new THREE.MeshBasicMaterial( { color: 0xffffff } );
      var material = new THREE.MeshBasicMaterial( { map: texture, side: THREE.DoubleSide, color: 0xffffff } );
      var cube = new THREE.Mesh( geometry, material );
      scene.add( cube );
      cube.rotation.z = Math.PI / 1;
      cube.rotation.y = -Math.PI / 1;
      camera.position.z = 32.7;
      
      renderer.render( scene, camera );
    }
  }
  ffmpeg.readPacket(function(){});
  ffmpeg.update(() => {
    console.log('update audio')
    playAudio()
  }, videoData => {
    // console.log('videoData', videoData)
    if (videoData) {
      texture.image.data = new Uint8Array(videoData);
      texture.needsUpdate = true;
    }
    renderer.render( scene, camera );
  });
}
function pause() {}

function playAudio() {
  if (info.audio) {
    let worker = new Worker('./audio-worker.js')

    let audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    let channels = +info.audio.channels
    let sampleRate = +info.audio.sampleRate
    let length = 5 * sampleRate // 左右声道各5秒音频缓冲
    let myAudioBuffer = audioCtx.createBuffer(channels, length, sampleRate);
    let nowBuffering1 = myAudioBuffer.getChannelData(0, 16, sampleRate);
    let nowBuffering2 = myAudioBuffer.getChannelData(1, 16, sampleRate);
    let timer
    let source = audioCtx.createBufferSource();
    let timeStart = 0
    source.buffer = myAudioBuffer;
    source.loop = true
    source.connect(audioCtx.destination);
    source.start(0);

    let prevSecond = 0;
    let contextTime
    let second
    let flagEnd
    function check() {
      
      // console.log(audioCtx.getOutputTimestamp())
        contextTime = audioCtx.getOutputTimestamp().contextTime
        second = Math.floor(contextTime)
        // ...... 设置音频时钟
        // console.log(contextTime)
        // const interval = Math.floor(1 / (2 * info.video.fps) * (audioCtx.getOutputTimestamp().performanceTime - timeStart))
        let interval = Math.floor((audioCtx.getOutputTimestamp().performanceTime - timeStart))
        // console.log(contextTime, interval)
        ffmpeg.updateAudioClock(interval)
        progressEl.style.width = contextTime * 1000 / info.video.duration * 100 + '%'
        // 根据时间差替换音频缓冲区内的数据
        if (second - prevSecond > 2) {
          worker.postMessage({
            code: 1,
            dataNeed: Math.abs(second - prevSecond) * sampleRate
          })
          prevSecond = second
        }
    }

    worker.onmessage = function(event) {
      // console.log(event.data)
      if (event.data.code == 3) {
        if (!timer) {
          timer = setInterval(check, 10)
          timeStart = audioCtx.getOutputTimestamp().performanceTime 
        }
        if (event.data.audioBufferLeft.length <= 0 && event.data.audioBufferRight.length <= 0) {
          clearInterval(timer)
          source.stop()
          source.disconnect()
          worker.terminate()
        }
        for (let i = 0; i < event.data.audioBufferLeft.length; i++) {
          let d = event.data.audioBufferLeft[i]
          nowBuffering1[(event.data.bufferIndex + i) % length] = ((d + 32768) % 65536 - 32768) / 32768.0
        }
      
        for (let i = 0; i < event.data.audioBufferRight.length; i++) {
          let d = event.data.audioBufferRight[i]
          nowBuffering2[(event.data.bufferIndex + i) % length] = ((d + 32768) % 65536 - 32768) / 32768.0
        }
      } else if (event.data.code == 4) {
        clearInterval(timer)
        source.stop()
        source.disconnect()
        worker.terminate()
      }
    }

    // timer = setTimeout(() => {
    //   clearTimeout(timer)
      worker.postMessage({
        code: 2,
        bufferLength: length
      })
      
    // }, 1000)

    worker.postMessage({
      code: 1,
      dataNeed: length
    })
    
  }
}

module.exports = {
  on: event.on,
  once: event.once,
  off: event.off,
  play,
  pause
}