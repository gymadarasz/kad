const char* common_js = R"COMMON_JS(

// for old browsers
if (!Date.now) {
    Date.now = function() { return new Date().getTime(); }
}

// ajax helpers

function toURI(obj) {
    var str = "";
    for (var key in obj) {
        if (str != "") str += "&";
        str += key + "=" + encodeURIComponent(obj[key]);
    }
    return str;
}

function ajax(method, url, data = {}, success = null, failure = null, timestamp = 'ts', async = true) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
        if (this.readyState == 4) {
            if (this.status == 200) {
                success ? success(this) : messageSuccess(this);
            } else { 
                failure ? failure(this) : messageFailed(this);
            }
        }
    }
    if (timestamp) data[timestamp] = Date.now();
    xhttp.open(method, url + '?' + toURI(data), async);
    xhttp.send();
}

// ui helpers

function hide(id) {
    document.getElementById(id).style.display = 'none';
}

function show(id) {
    document.getElementById(id).style.display = 'block';
}

function reload() {
    document.location.href = document.location.href;
}

function message(msg, type) {
    var id = 'message';
    msgelm = document.getElementById(id);
    msgelm.classList.remove('success');
    msgelm.classList.remove('error');
    msgelm.classList.add(type);
    show(id);
}

function messageSuccess(resp) {
    var msg = resp.responseText.replace(/\s/g, "X");
    if (msg) message(msg, 'success');
    else console.info(resp);
}

function messageFailed(resp) {
    var msg = resp.responseText.replace(/\s/g, "X");
    if (msg) message(msg, 'error');
    else console.error(resp);
}

)COMMON_JS";