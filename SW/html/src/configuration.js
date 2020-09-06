const names = ['version', 'name', 'ap1s', 'ap1p', 'ap2s', 'ap2p', 'ipAddr', 'ipMask', 'ipGateway', 'ipDNS'];
class Configuration {
    constructor() {
        this.initialize();
    }

    initialize() {
        for (let i = 0; i < names.length; ++i) {
            this[names[i]] = '';
        }
        this.version = 2;
    }

    fromString(jStr) {
        this.initialize();
        let cfg = JSON.parse(jStr);
        for (const [key, value] of Object.entries(cfg)) {
            if (this.hasOwnProperty(key)) {
                this[key] = value;
            }
        }
        this.toPage();
    }

    toPage() {
        for (const [key, value] of Object.entries(this)) {
            let a = document.getElementById('p' + key);
            if (a !== null) {
                a.value = value;
            }
        }
    }

    fromPagetoString() {
        for (const key of Object.keys(this)) {
            let a = document.getElementById('p' + key);
            if (a !== null) {
                this[key] = a.value;
            }
        }
        return JSON.stringify(this);
    }
};

export default Configuration;
