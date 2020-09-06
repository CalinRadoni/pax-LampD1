class ConfigPage {
    constructor() {
        this.pdiv = null;
    }

    set_parent_div(parentDiv) {
        this.pdiv = parentDiv;
    }
    render() {
        if (!this.pdiv) return;

        let xstr = '';

        xstr = xstr + '<h3>Configuration</h3>';
        xstr = xstr + 'Version: <input id="pversion" type="text" value=""><br/>';
        xstr = xstr + 'Name: <input id="pname" type="text" value=""><br/>';
        xstr = xstr + 'SSID: <input id="pap1s" type="text" value=""><br/>';
        xstr = xstr + 'Pass: <input id="pap1p" type="password" value=""><br/>';
        xstr = xstr + 'Backup SSID : <input id="pap2s" type="text" value=""><br/>';
        xstr = xstr + 'Backup Pass: <input id="pap2p" type="password" value=""><br/>';
        xstr = xstr + '<input type="button" value="Reload" onclick="GetConfig()" />&nbsp;';
        xstr = xstr + '<input type="button" value="Save" onclick="SaveConfig()" /><br/>';
        xstr = xstr + '<span id="csi"></span>';

        this.pdiv.innerHTML = xstr;
    }
};

export default ConfigPage;
