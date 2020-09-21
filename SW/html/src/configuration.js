const configNames = ['version', 'name', 'ap1s', 'ap1p', 'ap2s', 'ap2p', 'ipAddr', 'ipMask', 'ipGateway', 'ipDNS'];
class Configuration {
    constructor() {
        this.initialize();
    }

    initialize() {
        this.myVersion = 2;

        for (let i = 0; i < configNames.length; ++i) {
            this[configNames[i]] = '';
        }
        this.version = this.myVersion;
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
                if (key === 'version') {
                    a.innerHTML = 'Configuration version ' + value;
                }
                else {
                    a.value = value;
                }
            }
        }
    }

    fromPagetoString() {
        for (const key of Object.keys(this)) {
            let a = document.getElementById('p' + key);
            if (a !== null) {
                if (key === 'version') {
                    this.version = this.myVersion;
                }
                else {
                    this[key] = a.value;
                }
            }
        }
        return JSON.stringify(this);
    }
}
