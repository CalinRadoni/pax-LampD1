class FirmwarePage {
    constructor() {
        this.pdiv = null;
    }

    set_parent_div(parentDiv) {
        this.pdiv = parentDiv;
    }
    render() {
        if (!this.pdiv) return;

        let xstr = '';

        xstr = xstr + '<h3>Firmware</h3>';
        xstr = xstr + '<input type="file" id="fwf" accept=".bin" style="display:none" onchange="swfC(this)" />';
        xstr = xstr + '<input type="button" value="Select firmware file" onclick="document.getElementById("fwf").click();" /> &gt;&gt;';
        xstr = xstr + '<input type="button" value="Upload firmware" onclick="UpFW()" /> &gt;&gt;';
        xstr = xstr + '<input type="button" value="Reset" onclick="ResetBoard()" /><br/>';
        xstr = xstr + '<span id="swf"></span><br/>';
        xstr = xstr + '<span id="swi"></span><br/>';

        this.pdiv.innerHTML = xstr;
    }
}
