'use strict';

/**
 * (1) App
 * (2) Utils
 */



/**
 * App
 */
function App(canvas, length) {
    this.canvas = canvas;
    this.ctx = canvas.getContext('2d');
    
    this.length = length;
    this.halfLength = Math.floor(length / 2);
    this.pixels = new Uint32Array(2 * this.halfLength);
    // this.pixels = new Array(2 * this.halfLength);
    // this.pixels.fill('#000');
    
    // Render loop
    this.fps = 30;
    this.interval = 1000 / this.fps;
    this.requestId;
    this.last;
    
    this.init();
}

App.prototype.init = function() {
    // ...
};

App.prototype.start = function() {
    if (!this.requestId) {
        this.last = window.performance.now();
        this._frame();
    }
};

App.prototype.stop = function() {
    if (this.requestId) {
        cancelAnimationFrame(this.requestId);
        this.requestId = undefined;
    }
};

App.prototype._frame = function() {
    var self = this;
    var now = window.performance.now();
    var dt = now - this.last;
    
    if (dt > this.interval) {
        this._update(dt / 1000, now / 1000);
        this.last = now - (dt % this.interval);
    }
    
    self.requestId = requestAnimationFrame(function() {
        self._frame();
    });
};

var next = 0;

App.prototype._update = function(dt, now) {
    // if (now >= next) {
        this._clear();
        
        var p = Math.randInt(0, this.length);
        this.pixels[p] = Math.rgbToInt(255, 127, 0);
        
        // Left
        for (var i = 0; i < this.halfLength; i++) {
            this.ctx.fillStyle = Math.intToHex(this.pixels[i]);
            this.ctx.fillRect(0, (i * 11) + 1, 10, 10);
        }
        
        // Right
        for (i = 0; i < this.halfLength; i++) {
            this.ctx.fillStyle = Math.intToHex(this.pixels[i + this.halfLength]);
            this.ctx.fillRect(this.canvas.width - 10, (i * 11) + 1, 10, 10);
        }
        // next = now + 0.1;
    // }
    
};

App.prototype._clear = function() {
    this.ctx.clearRect(0, 0, this.canvas.width, this.canvas.height);
}



/**
 * Utils
 */
Math.radians = function(degrees) {
    return degrees * Math.PI / 180;
};

Math.degrees = function(radians) {
    return radians * 180 / Math.PI;
};

Math.randInt = function(min, max) {
    return Math.floor(Math.random() * (max - min + 1)) + min;
}

Math.intToRgb = function(i) {
    return {
        r: (i & 0xff0000) >> 16, 
        g: (i & 0x00ff00) >> 8, 
        b: (i & 0x0000ff)
    };
}

Math.rgbToInt = function(r, g, b) {
    return (r << 16) + (g << 8) + (b);
}

Math.intToHex = function(i) {
    return '#' + ('00000' + (i | 0).toString(16)).substr(-6); 
}