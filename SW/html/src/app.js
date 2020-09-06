/******/ (function(modules) { // webpackBootstrap
/******/ 	// The module cache
/******/ 	var installedModules = {};
/******/
/******/ 	// The require function
/******/ 	function __webpack_require__(moduleId) {
/******/
/******/ 		// Check if module is in cache
/******/ 		if(installedModules[moduleId]) {
/******/ 			return installedModules[moduleId].exports;
/******/ 		}
/******/ 		// Create a new module (and put it into the cache)
/******/ 		var module = installedModules[moduleId] = {
/******/ 			i: moduleId,
/******/ 			l: false,
/******/ 			exports: {}
/******/ 		};
/******/
/******/ 		// Execute the module function
/******/ 		modules[moduleId].call(module.exports, module, module.exports, __webpack_require__);
/******/
/******/ 		// Flag the module as loaded
/******/ 		module.l = true;
/******/
/******/ 		// Return the exports of the module
/******/ 		return module.exports;
/******/ 	}
/******/
/******/
/******/ 	// expose the modules object (__webpack_modules__)
/******/ 	__webpack_require__.m = modules;
/******/
/******/ 	// expose the module cache
/******/ 	__webpack_require__.c = installedModules;
/******/
/******/ 	// define getter function for harmony exports
/******/ 	__webpack_require__.d = function(exports, name, getter) {
/******/ 		if(!__webpack_require__.o(exports, name)) {
/******/ 			Object.defineProperty(exports, name, { enumerable: true, get: getter });
/******/ 		}
/******/ 	};
/******/
/******/ 	// define __esModule on exports
/******/ 	__webpack_require__.r = function(exports) {
/******/ 		if(typeof Symbol !== 'undefined' && Symbol.toStringTag) {
/******/ 			Object.defineProperty(exports, Symbol.toStringTag, { value: 'Module' });
/******/ 		}
/******/ 		Object.defineProperty(exports, '__esModule', { value: true });
/******/ 	};
/******/
/******/ 	// create a fake namespace object
/******/ 	// mode & 1: value is a module id, require it
/******/ 	// mode & 2: merge all properties of value into the ns
/******/ 	// mode & 4: return value when already ns object
/******/ 	// mode & 8|1: behave like require
/******/ 	__webpack_require__.t = function(value, mode) {
/******/ 		if(mode & 1) value = __webpack_require__(value);
/******/ 		if(mode & 8) return value;
/******/ 		if((mode & 4) && typeof value === 'object' && value && value.__esModule) return value;
/******/ 		var ns = Object.create(null);
/******/ 		__webpack_require__.r(ns);
/******/ 		Object.defineProperty(ns, 'default', { enumerable: true, value: value });
/******/ 		if(mode & 2 && typeof value != 'string') for(var key in value) __webpack_require__.d(ns, key, function(key) { return value[key]; }.bind(null, key));
/******/ 		return ns;
/******/ 	};
/******/
/******/ 	// getDefaultExport function for compatibility with non-harmony modules
/******/ 	__webpack_require__.n = function(module) {
/******/ 		var getter = module && module.__esModule ?
/******/ 			function getDefault() { return module['default']; } :
/******/ 			function getModuleExports() { return module; };
/******/ 		__webpack_require__.d(getter, 'a', getter);
/******/ 		return getter;
/******/ 	};
/******/
/******/ 	// Object.prototype.hasOwnProperty.call
/******/ 	__webpack_require__.o = function(object, property) { return Object.prototype.hasOwnProperty.call(object, property); };
/******/
/******/ 	// __webpack_public_path__
/******/ 	__webpack_require__.p = "";
/******/
/******/
/******/ 	// Load entry module and return exports
/******/ 	return __webpack_require__(__webpack_require__.s = "./src/main.js");
/******/ })
/************************************************************************/
/******/ ({

/***/ "./src/config_page.js":
/*!****************************!*\
  !*** ./src/config_page.js ***!
  \****************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\nclass ConfigPage {\n    constructor() {\n        this.pdiv = null;\n    }\n\n    set_parent_div(parentDiv) {\n        this.pdiv = parentDiv;\n    }\n    render() {\n        if (!this.pdiv) return;\n\n        let xstr = '';\n\n        xstr = xstr + '<h3>Configuration</h3>';\n        xstr = xstr + 'Version: <input id=\"pversion\" type=\"text\" value=\"\"><br/>';\n        xstr = xstr + 'Name: <input id=\"pname\" type=\"text\" value=\"\"><br/>';\n        xstr = xstr + 'SSID: <input id=\"pap1s\" type=\"text\" value=\"\"><br/>';\n        xstr = xstr + 'Pass: <input id=\"pap1p\" type=\"password\" value=\"\"><br/>';\n        xstr = xstr + 'Backup SSID : <input id=\"pap2s\" type=\"text\" value=\"\"><br/>';\n        xstr = xstr + 'Backup Pass: <input id=\"pap2p\" type=\"password\" value=\"\"><br/>';\n        xstr = xstr + '<input type=\"button\" value=\"Reload\" onclick=\"GetConfig()\" />&nbsp;';\n        xstr = xstr + '<input type=\"button\" value=\"Save\" onclick=\"SaveConfig()\" /><br/>';\n        xstr = xstr + '<span id=\"csi\"></span>';\n\n        this.pdiv.innerHTML = xstr;\n    }\n};\n\n/* harmony default export */ __webpack_exports__[\"default\"] = (ConfigPage);\n\n\n//# sourceURL=webpack:///./src/config_page.js?");

/***/ }),

/***/ "./src/configuration.js":
/*!******************************!*\
  !*** ./src/configuration.js ***!
  \******************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\nconst names = ['version', 'name', 'ap1s', 'ap1p', 'ap2s', 'ap2p', 'ipAddr', 'ipMask', 'ipGateway', 'ipDNS'];\nclass Configuration {\n    constructor() {\n        this.initialize();\n    }\n\n    initialize() {\n        for (let i = 0; i < names.length; ++i) {\n            this[names[i]] = '';\n        }\n        this.version = 2;\n    }\n\n    fromString(jStr) {\n        this.initialize();\n        let cfg = JSON.parse(jStr);\n        for (const [key, value] of Object.entries(cfg)) {\n            if (this.hasOwnProperty(key)) {\n                this[key] = value;\n            }\n        }\n        this.toPage();\n    }\n\n    toPage() {\n        for (const [key, value] of Object.entries(this)) {\n            let a = document.getElementById('p' + key);\n            if (a !== null) {\n                a.value = value;\n            }\n        }\n    }\n\n    fromPagetoString() {\n        for (const key of Object.keys(this)) {\n            let a = document.getElementById('p' + key);\n            if (a !== null) {\n                this[key] = a.value;\n            }\n        }\n        return JSON.stringify(this);\n    }\n};\n\n/* harmony default export */ __webpack_exports__[\"default\"] = (Configuration);\n\n\n//# sourceURL=webpack:///./src/configuration.js?");

/***/ }),

/***/ "./src/firmware_page.js":
/*!******************************!*\
  !*** ./src/firmware_page.js ***!
  \******************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\nclass FirmwarePage {\n    constructor() {\n        this.pdiv = null;\n    }\n\n    set_parent_div(parentDiv) {\n        this.pdiv = parentDiv;\n    }\n    render() {\n        if (!this.pdiv) return;\n\n        let xstr = '';\n\n        xstr = xstr + '<h3>Firmware</h3>';\n        xstr = xstr + '<input type=\"file\" id=\"fwf\" accept=\".bin\" style=\"display:none\" onchange=\"swfC(this)\" />';\n        xstr = xstr + '<input type=\"button\" value=\"Select firmware file\" onclick=\"document.getElementById(\"fwf\").click();\" /> &gt;&gt;';\n        xstr = xstr + '<input type=\"button\" value=\"Upload firmware\" onclick=\"UpFW()\" /> &gt;&gt;';\n        xstr = xstr + '<input type=\"button\" value=\"Reset\" onclick=\"ResetBoard()\" /><br/>';\n        xstr = xstr + '<span id=\"swf\"></span><br/>';\n        xstr = xstr + '<span id=\"swi\"></span><br/>';\n\n        this.pdiv.innerHTML = xstr;\n    }\n};\n\n/* harmony default export */ __webpack_exports__[\"default\"] = (FirmwarePage);\n\n\n//# sourceURL=webpack:///./src/firmware_page.js?");

/***/ }),

/***/ "./src/home_page.js":
/*!**************************!*\
  !*** ./src/home_page.js ***!
  \**************************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\nclass HomePage {\n    constructor() {\n        this.pdiv = null;\n    }\n\n    CreateSmallColorWheel(id) {\n        let nRGB = [\"800000\",\"804000\",\"808000\",\"408000\",\"008000\",\"008040\",\"008080\",\"004080\",\"000080\",\"400080\",\"800080\",\"800040\"];\n        let nRYB = [\"7F1208\",\"7F2F05\",\"7C4B01\",\"7F660C\",\"7F7F19\",\"596B18\",\"325718\",\"1A3E4C\",\"01227F\",\"22126B\",\"440159\",\"600A2F\"];\n        let pRGB = [\"FF0000\",\"FF8000\",\"FFFF00\",\"80FF00\",\"00FF00\",\"00FF80\",\"00FFFF\",\"0080FF\",\"0000FF\",\"8000FF\",\"FF00FF\",\"FF0080\"];\n        let pRYB = [\"FE2712\",\"FC600A\",\"FB9902\",\"FCCC1A\",\"FEFE33\",\"B2D732\",\"66B032\",\"347C98\",\"0247FE\",\"4424D6\",\"8601AF\",\"C21460\"];\n        let nn = id==0 ? nRGB : nRYB;\n        let pp = id==0 ? pRGB : pRYB;\n        let str = '';\n        for ( let i = 0; i < pp.length; i++) {\n            if ((i % 6) == 0) {\n                if (i != 0) {\n                    str += '</div>';\n                }\n                str += '<div class=\"btnr\">';\n            }\n            str += '<div class=\"btnc\" name=\"' + nn[i] + '\" onclick=\"PClick(this)\" style=\"background-color:#' + pp[i] +';\"></div>';\n        }\n        str += '</div>';\n        return str;\n    }\n\n    set_parent_div(parentDiv) {\n        this.pdiv = parentDiv;\n    }\n    render() {\n        if (!this.pdiv) return;\n\n        let str = '';\n\n        str = str + '<h3>AAA Enter color code in hex</h3>';\n        str = str + '<input id=\"CCode\" type=\"text\" value=\"\" maxlength=\"8\" size=\"8\">&nbsp';\n        str = str + '<input type=\"button\" value=\"Set\" onclick=\"SetColor()\" />&nbsp';\n        str = str + '<input type=\"button\" value=\"Off\" onclick=\"TurnOff()\" /><br/>';\n        str = str + '<h3>RGB</h3><div>' + this.CreateSmallColorWheel(0) + '</div><br/>';\n        str = str + '<h3>RYB</h3><div>' + this.CreateSmallColorWheel(1) + '</div>';\n\n        this.pdiv.innerHTML = str;\n    }\n};\n\n/* harmony default export */ __webpack_exports__[\"default\"] = (HomePage);\n\n\n//# sourceURL=webpack:///./src/home_page.js?");

/***/ }),

/***/ "./src/logger.js":
/*!***********************!*\
  !*** ./src/logger.js ***!
  \***********************/
/*! exports provided: default */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\nclass Logger {\n    constructor() {\n        this.messages = [];\n        this.maxMsg = 5;\n        this.pdiv = undefined;\n    }\n\n    _addMessage(msg) {\n        this.messages.unshift(msg);\n        if (this.messages.length > this.maxMsg) {\n            this.messages.pop();\n        }\n        this.render();\n    }\n\n    info(str) {\n        let message = { \"type\": \"info\", \"date\": Date.now(), \"msg\": str };\n        this._addMessage(message);\n    }\n    warning(str) {\n        let message = { \"type\": \"warning\", \"date\": Date.now(), \"msg\": str };\n        this._addMessage(message);\n    }\n    error(str) {\n        let message = { \"type\": \"error\", \"date\": Date.now(), \"msg\": str };\n        this._addMessage(message);\n    }\n\n    set_parent_div(parentDiv) {\n        this.pdiv = parentDiv;\n    }\n    render() {\n        if (this.pdiv === undefined) return;\n\n        let xstr = '';\n        for (let i = 0; i < this.messages.length; ++i){\n            let message = this.messages[i];\n            let d = new Date();\n            d.setTime(message.date);\n\n            let str = '<p class=\"log-' + message.type + '\">' +\n                d.toISOString() +\n                ' ' +\n                message.msg +\n                '</p>';\n\n            xstr = xstr + str;\n        }\n        this.pdiv.innerHTML = xstr;\n    }\n};\n\n/* harmony default export */ __webpack_exports__[\"default\"] = (Logger);\n\n\n//# sourceURL=webpack:///./src/logger.js?");

/***/ }),

/***/ "./src/main.js":
/*!*********************!*\
  !*** ./src/main.js ***!
  \*********************/
/*! no exports provided */
/***/ (function(module, __webpack_exports__, __webpack_require__) {

"use strict";
eval("__webpack_require__.r(__webpack_exports__);\n/* harmony import */ var _logger__WEBPACK_IMPORTED_MODULE_0__ = __webpack_require__(/*! ./logger */ \"./src/logger.js\");\n/* harmony import */ var _home_page__WEBPACK_IMPORTED_MODULE_1__ = __webpack_require__(/*! ./home_page */ \"./src/home_page.js\");\n/* harmony import */ var _firmware_page__WEBPACK_IMPORTED_MODULE_2__ = __webpack_require__(/*! ./firmware_page */ \"./src/firmware_page.js\");\n/* harmony import */ var _config_page__WEBPACK_IMPORTED_MODULE_3__ = __webpack_require__(/*! ./config_page */ \"./src/config_page.js\");\n/* harmony import */ var _configuration__WEBPACK_IMPORTED_MODULE_4__ = __webpack_require__(/*! ./configuration */ \"./src/configuration.js\");\n\n\n\n\n\n\nclass App {\n    constructor() {\n        this.logger = new _logger__WEBPACK_IMPORTED_MODULE_0__[\"default\"]();\n        this.homePage = new _home_page__WEBPACK_IMPORTED_MODULE_1__[\"default\"]();\n        this.config = new _configuration__WEBPACK_IMPORTED_MODULE_4__[\"default\"]();\n        this.configPage = new _config_page__WEBPACK_IMPORTED_MODULE_3__[\"default\"]();\n        this.firmwarePage = new _firmware_page__WEBPACK_IMPORTED_MODULE_2__[\"default\"]();\n    }\n\n    Initialize() {\n        this.logger.set_parent_div(document.getElementById('LOG'));\n        this.logger.info('Test info');\n        this.logger.warning('Test warning');\n        this.logger.error('Test error');\n\n        let appel = document.getElementById('APP');\n\n        this.homePage.set_parent_div(appel);\n        this.configPage.set_parent_div(appel);\n        this.firmwarePage.set_parent_div(appel);\n\n        this.GetConfig();\n\n        this.HashHandler();\n        window.addEventListener('hashchange', this.HashHandler, false);\n    }\n\n    SaveConfig() {\n        let xhr = new XMLHttpRequest();\n        xhr.onload = function() {\n            if (xhr.readyState === xhr.DONE) {\n                if (xhr.status === 200) {\n                    this.logger.info(xhr.responseText);\n                }\n                else {\n                    this.logger.error(xhr.status + \" \" + xhr.responseText);\n                }\n            }\n        }\n        xhr.onerror = function() { this.logger.error(\"Send error\"); }\n        xhr.onabort = function() { this.logger.warning(\"Send canceled\"); }\n\n        xhr.open(\"POST\", \"/config.json\", true);\n        xhr.setRequestHeader(\"Content-Type\", \"application/json;charset=UTF-8\");\n        xhr.send(this.config.fromPagetoString());\n    }\n\n    GetConfig() {\n        let xhr = new XMLHttpRequest();\n        xhr.onload = function() {\n            if (xhr.readyState === xhr.DONE) {\n                if (xhr.status === 200) {\n                    this.config.fromString(xhr.responseText);\n                }\n            }\n        };\n        xhr.open(\"GET\", \"/config.json\", true);\n        xhr.send();\n    }\n\n    SendCmd(cmdID) {\n        let xhr = new XMLHttpRequest();\n        xhr.onload = function() {\n            if (xhr.readyState === xhr.DONE) {\n                if (xhr.status === 200) {\n                    this.logger.info(xhr.responseText);\n                }\n                else {\n                    this.logger.error(xhr.status + \" \" + xhr.responseText);\n                }\n            }\n        }\n        xhr.onerror = function() { this.logger.error(\"Send error\"); }\n        xhr.onabort = function() { this.logger.warning(\"Send canceled\"); }\n\n        let data = document.getElementById('CCode').value;\n        let str = JSON.stringify({ \"cmd\": cmdID, \"data\": data });\n\n        xhr.open(\"POST\", \"/cmd.json\", true);\n        xhr.setRequestHeader(\"Content-Type\", \"application/json;charset=UTF-8\");\n        xhr.send(str);\n    }\n\n    SetColor() { this.SendCmd(2); }\n\n    TurnOff()  { this.SendCmd(1); }\n\n    PClick(cdiv) {\n        document.getElementById('CCode').value = cdiv.getAttribute(\"name\");\n        this.SendCmd(1);\n    }\n\n    ResetBoard() { this.SendCmd(0xFE); }\n\n    swfC(ii) {\n        if (!ii) return;\n        if (!ii.files) return;\n        let f = ii.files[0];\n        if (!f) return;\n        let d = document.getElementById('swf');\n        if (d) {\n            d.innerHTML = f.name + \"<br/>size: \" + f.size + \" bytes\";\n        }\n    }\n\n    UpFW() {\n        let fs = document.getElementById('fwf');\n        if (!(fs && fs.files)) return;\n\n        let f = fs.files[0];\n        if (!f) return;\n\n        let xhr = new XMLHttpRequest();\n        xhr.onreadystatechange = function() {\n            if (xhr.readyState === xhr.DONE) {\n                if (xhr.status === 200) {\n                    this.logger.info(xhr.responseText);\n                }\n                else {\n                    this.logger.error(xhr.status + \" \" + xhr.responseText);\n                }\n            }\n        }\n\n        var upp = document.getElementById('swi');\n        xhr.upload.addEventListener(\"progress\", function(ev) {\n            if (ev.lengthComputable) {\n                let percent = 100 * ev.loaded / ev.total | 0;\n                upp.innerHTML = percent + \"%\";\n            }\n        });\n\n        xhr.open(\"POST\", \"/update\", true);\n        xhr.send(f);\n    }\n\n    HashHandler() {\n        if (location.hash === \"#home\") {\n            app.homePage.render();\n            return;\n        }\n\n        if (location.hash === \"#config\") {\n            app.configPage.render();\n            return;\n        }\n\n        if (location.hash === \"#firmware\") {\n            app.firmwarePage.render();\n            return;\n        }\n\n        // treat everything else like #home\n        app.homePage.render();\n    }\n}\n\nvar app = new App();\napp.Initialize();\n\n\n//# sourceURL=webpack:///./src/main.js?");

/***/ })

/******/ });