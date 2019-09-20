const ffmpeg = require('./build/Release/ffmpeg.node');
const EventEmitter = require('events');
const path = require('path');

const event = new EventEmitter();

let info = {}, videoInitialized = false, initialized = false;

let worker
let audioCtx, length;
let source, gainNode;
let timer
let audioPaused = false, audioPlaying = false;

var videoWebGL = {
  scene: null,
  cube: null,
  renderer: null,
  camera: null,
  width: null,
  height: null,
  ratio: null,
  data: null,
  texture: null,
  geometry: null,
  material: null,
};

function stop() {
  if (!initialized) {
    return ;
  }
  audioPlaying = false;
  // 关闭webAudio音频上下文和时钟更新、关闭webWorker音频缓冲线程
  clearInterval(timer)
  timer = null;
  if (worker) {
    worker.terminate()
    worker = null;
    // worker.postMessage({
    //   code: 'reset'
    // });
  }
  if (source) {
    source.stop();
    source.disconnect();
    gainNode.disconnect();
    gainNode = null;
    source = null;
  }
  if (audioCtx) {
    audioCtx.close();
    audioCtx = null;
  }
  ffmpeg.suspend();
  // 关闭c++插件的readPacket后台线程及update视频图像推送线程、调用c++插件关闭ffmpeg解码上下文
  ffmpeg.destroy();
}
function play(filename) {
  // 停止正在播放的视频
  stop()
  audioPlaying = true;
  audioPaused = false;
  console.log('stopped...')
  ffmpeg.init(filename)
  info = ffmpeg.getInfo()
  console.log('info', info)
  if (!initialized) {
    initialized = true
    if (info.video) {
      videoWebGL.scene = new THREE.Scene();
      videoWebGL.camera = new THREE.PerspectiveCamera( 45, window.innerWidth/window.innerHeight, 0.1, 1000 );
      videoWebGL.renderer = new THREE.WebGLRenderer();
      videoWebGL.renderer.setSize( window.innerWidth, window.innerHeight );
      document.querySelector('#playerContainer').appendChild( videoWebGL.renderer.domElement );
      window.addEventListener( 'resize', onWindowResize, false );
      
      function onWindowResize(){
        videoWebGL.camera.aspect = window.innerWidth / window.innerHeight;
        videoWebGL.camera.updateProjectionMatrix();
        videoWebGL.renderer.setSize( window.innerWidth, window.innerHeight );
      }
              
      videoWebGL.width = +info.video.width;
      videoWebGL.height = +info.video.height;
      videoWebGL.ratio = videoWebGL.width / videoWebGL.height
      videoWebGL.data = new Uint8Array(videoWebGL.width * videoWebGL.height * 3)
      videoWebGL.texture = new THREE.DataTexture(videoWebGL.data, videoWebGL.width, videoWebGL.height, THREE.RGBFormat);
      videoWebGL.geometry = new THREE.PlaneGeometry( 27 * videoWebGL.ratio, 27, 1, 1 )
      videoWebGL.material = new THREE.MeshBasicMaterial( { map: videoWebGL.texture, side: THREE.DoubleSide, color: 0xffffff } );
      videoWebGL.cube = new THREE.Mesh(videoWebGL.geometry, videoWebGL.material);
      videoWebGL.scene.add(videoWebGL.cube);
      videoWebGL.cube.rotation.z = Math.PI / 1;
      videoWebGL.cube.rotation.y = -Math.PI / 1;
      
      videoWebGL.camera.position.z = 32.7;
      videoWebGL.renderer.render(videoWebGL.scene, videoWebGL.camera);
    }
    ffmpeg.readPacket(function(){});
    ffmpeg.update(() => {
      console.log('update audio ==================')
      playAudio()
      // initialized = true;
    }, videoData => {
      // console.log('videoData ---------------------')
      if (videoData) {
        videoWebGL.texture.image.data = new Uint8Array(videoData);
        videoWebGL.texture.needsUpdate = true;
      }
      videoWebGL.renderer.render(videoWebGL.scene, videoWebGL.camera);
    });
  } else {
    videoWebGL.scene.remove(videoWebGL.cube);
    videoWebGL.width = +info.video.width;
    videoWebGL.height = +info.video.height;
    videoWebGL.ratio = videoWebGL.width / videoWebGL.height
    videoWebGL.data = new Uint8Array(videoWebGL.width * videoWebGL.height * 3)
    videoWebGL.texture = new THREE.DataTexture(videoWebGL.data, videoWebGL.width, videoWebGL.height, THREE.RGBFormat);
    videoWebGL.geometry = new THREE.PlaneGeometry( 27 * videoWebGL.ratio, 27, 1, 1 )
    videoWebGL.material = new THREE.MeshBasicMaterial( { map: videoWebGL.texture, side: THREE.DoubleSide, color: 0xffffff } );
    videoWebGL.cube = new THREE.Mesh(videoWebGL.geometry, videoWebGL.material);
    videoWebGL.scene.add(videoWebGL.cube);
    videoWebGL.cube.rotation.z = Math.PI / 1;
    videoWebGL.cube.rotation.y = -Math.PI / 1;
    
    videoWebGL.camera.position.z = 32.7;
    videoWebGL.renderer.render(videoWebGL.scene, videoWebGL.camera);
    
    ffmpeg.resume();
  }
}
function pause() {
  if (audioCtx) {
    audioPaused = true;
    audioCtx.suspend();
    event.emit('pause')
  }
}
function resume() {
  if (audioCtx) {
    audioCtx.resume();
    audioPaused = false;
    event.emit('resume')
  }
}

function playAudio() {
  if (info.audio) {

    worker = new Worker(path.resolve(__dirname, './src/utils/audio-worker.js'))

    audioCtx = new (window.AudioContext || window.webkitAudioContext)();
    gainNode = audioCtx.createGain();
    let channels = +info.audio.channels
    let sampleRate = +info.audio.sampleRate
    length = 5 * sampleRate // 左右声道各5秒音频缓冲
    let myAudioBuffer = audioCtx.createBuffer(channels, length, sampleRate);
    let nowBuffering1 = myAudioBuffer.getChannelData(0, 16, sampleRate);
    let nowBuffering2 = myAudioBuffer.getChannelData(1, 16, sampleRate);
    source = audioCtx.createBufferSource();
    // let timeStart = 0
    source.buffer = myAudioBuffer;
    source.loop = true
    // source.connect(audioCtx.destination);
    source.connect(gainNode);
    gainNode.connect(audioCtx.destination);
    source.start(0);

    let prevSecond = 0;
    let contextTime
    let second
    function check() {
      contextTime = audioCtx.getOutputTimestamp().contextTime
      second = Math.floor(contextTime)
      // 设置音频时钟
      // let interval = Math.floor((audioCtx.getOutputTimestamp().performanceTime - timeStart))
      // console.log(contextTime, interval)
      info.currentTime = Math.floor(contextTime * 1000);
      ffmpeg.updateAudioClock(info.currentTime);
      if (info.currentTime <= info.video.duration) {
        event.emit('progress', info.currentTime);
      } else {
        stop();
        event.emit('ended');
      }
      // progressEl.style.width = contextTime * 1000 / info.video.duration * 100 + '%'
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
          console.log('create audio timer');
          timer = setInterval(check, 10)
          // timeStart = audioCtx.getOutputTimestamp().performanceTime 
        }
        // if (event.data.audioBufferLeft.length <= 0 && event.data.audioBufferRight.length <= 0) {
        //   clearInterval(timer)
        //   source.stop()
        //   source.disconnect()
        //   worker.terminate()
        // }
        for (let i = 0; i < event.data.audioBufferLeft.length; i++) {
          let d = event.data.audioBufferLeft[i]
          nowBuffering1[(event.data.bufferIndex + i) % length] = ((d + 32768) % 65536 - 32768) / 32768.0
        }
      
        for (let i = 0; i < event.data.audioBufferRight.length; i++) {
          let d = event.data.audioBufferRight[i]
          nowBuffering2[(event.data.bufferIndex + i) % length] = ((d + 32768) % 65536 - 32768) / 32768.0
        }
      } else if (event.data.code == 4) {
        // clearInterval(timer)
        // source.stop()
        // source.disconnect()
        // worker.terminate()
      } else if (event.data.code == 'worker-ready') {
        worker.postMessage({
          code: 1,
          dataNeed: length
        })
      }
    }

    worker.postMessage({
      code: 2,
      bufferLength: length
    })
  
  }
}

let ex = {
  // api
  on() {
    event.on.apply(event, arguments);
  },
  resume,
  play,
  pause
}
Object.defineProperties(ex, {
  volume: {
    get() {
      if (audioCtx && gainNode && audioPlaying) {
        return gainNode.gain.value;
      }
    },
    set(val) {
      if (audioCtx && gainNode && audioPlaying) {
        // gainNode.gain.value = val;
        gainNode.gain.setValueAtTime(val, audioCtx.currentTime);
      }
    }
  },
  playing: {
    get() {
      return audioPlaying;
    },
    set(){}
  },
  paused: {
    get() {
      return audioPaused;
    },
    set(){}
  },
  video: {
    get() {
      return info.video;
    },
    set() {}
  },
  audio: {
    get() {
      return info.audio;
    },
    set() {}
  },
  duration: {
    get() {
      return info.video.duration || 0;
    },
    set() {}
  },
  currentTime: {
    get() {
      return info.currentTime || 0;
    },
    set() {}
  }
})

module.exports = ex;