const char* panel_html = R"PANEL_HTML(
<html>
    <head>
        <title>Control panel</title>
        <style>
            
            {{ common_css }}

            body {
                font-family: "Times New Roman", Times, serif;
                text-align: center;
            }

            h1 { 
                text-shadow: 0px 0px 10px rgba(150, 150, 150, 1);
                min-height: 3.5em;
            }

            .tile {
                width: 250px;
                display: block;
                float: left;
                margin: 20px;
            }

            .tile > div {
                color: white;
            }

            .tile .vertical {
                margin: 10px auto;
                width: 100px;
                height: 400px;
                position: relative;
                font-size: 1.5em;
                box-shadow: inset 0px 0px 10px 0px rgba(51,51,51,0.8);
            }

            .tile .vertical .top {
                position: absolute;
                top: 10px;
                left: 50%;
                -ms-transform: translateX(-50%);
                transform: translateX(-50%);
            }

            .tile .vertical .bottom {
                position: absolute;
                bottom: 10px;
                left: 50%;
                -ms-transform: translateX(-50%);
                transform: translateX(-50%);
            }

            .tile .vertical.temperature {
                background: #f72000; /* Old browsers */
                background: -moz-linear-gradient(top,  #f72000 0%, #0046ea 100%); /* FF3.6-15 */
                background: -webkit-linear-gradient(top,  #f72000 0%,#0046ea 100%); /* Chrome10-25,Safari5.1-6 */
                background: linear-gradient(to bottom,  #f72000 0%,#0046ea 100%); /* W3C, IE10+, FF16+, Chrome26+, Opera12+, Safari7+ */
                filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#f72000', endColorstr='#0046ea',GradientType=0 ); /* IE6-9 */
            }

            .tile .vertical.oxygen {
                background: #0041f4; /* Old browsers */
                background: -moz-linear-gradient(top,  #0041f4 0%, #8ea8e5 100%); /* FF3.6-15 */
                background: -webkit-linear-gradient(top,  #0041f4 0%,#8ea8e5 100%); /* Chrome10-25,Safari5.1-6 */
                background: linear-gradient(to bottom,  #0041f4 0%,#8ea8e5 100%); /* W3C, IE10+, FF16+, Chrome26+, Opera12+, Safari7+ */
                filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#0041f4', endColorstr='#8ea8e5',GradientType=0 ); /* IE6-9 */
            }
        
            .tile .monitor {
                background: #0041f4; /* Old browsers */
                background: -moz-linear-gradient(top,  #0041f4 0%, #0041f4 94%, #8ea8e5 100%); /* FF3.6-15 */
                background: -webkit-linear-gradient(top,  #0041f4 0%,#0041f4 94%,#8ea8e5 100%); /* Chrome10-25,Safari5.1-6 */
                background: linear-gradient(to bottom,  #0041f4 0%,#0041f4 94%,#8ea8e5 100%); /* W3C, IE10+, FF16+, Chrome26+, Opera12+, Safari7+ */
                filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#0041f4', endColorstr='#8ea8e5',GradientType=0 ); /* IE6-9 */
                    
                margin: 10px auto;
                width: 220px;
                height: 129px;
                position: relative;

                font-size: 2em;
                box-shadow: inset 0px 0px 10px 0px rgba(1,1,1,1);
            }

            .tile .circle {
                width: 220px;
                height: 220px;
                border-radius: 50%;
                margin: 10px auto;
                position: relative;
                box-shadow: inset 0px 0px 10px 0px rgba(51,51,51,0.8);
            }

            .tile .middle {
                margin: 0;
                position: absolute;
                top: 40%;
                left: 50%;
                -ms-transform: translateX(-50%);
                transform: translateX(-50%);
            }

            .tile .circle.colour {
                background: #f94231; /* Old browsers */
                background: -moz-linear-gradient(top,  #f94231 0%, #edf72e 25%, #2ff93c 49%, #276bf4 71%, #f927f6 100%); /* FF3.6-15 */
                background: -webkit-linear-gradient(top,  #f94231 0%,#edf72e 25%,#2ff93c 49%,#276bf4 71%,#f927f6 100%); /* Chrome10-25,Safari5.1-6 */
                background: linear-gradient(to bottom,  #f94231 0%,#edf72e 25%,#2ff93c 49%,#276bf4 71%,#f927f6 100%); /* W3C, IE10+, FF16+, Chrome26+, Opera12+, Safari7+ */
                filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#f94231', endColorstr='#f927f6',GradientType=0 ); /* IE6-9 */
            }

            .tile .circle.stop {
                background: #eff248; /* Old browsers */
                background: -moz-linear-gradient(top,  #eff248 0%, #ecf400 84%, #fcc100 100%); /* FF3.6-15 */
                background: -webkit-linear-gradient(top,  #eff248 0%,#ecf400 84%,#fcc100 100%); /* Chrome10-25,Safari5.1-6 */
                background: linear-gradient(to bottom,  #eff248 0%,#ecf400 84%,#fcc100 100%); /* W3C, IE10+, FF16+, Chrome26+, Opera12+, Safari7+ */
                filter: progid:DXImageTransform.Microsoft.gradient( startColorstr='#eff248', endColorstr='#fcc100',GradientType=0 ); /* IE6-9 */
            }

            .tile .circle.stop .inner {
                width: 70%;
                height: 70%;
                position: relative;
                top: 10%;
                margin: 0 auto;
                border: 10px solid #D35A47;
                border-radius: 50%;
                background: #C40105;
            }

            .tile .monitor .middle {
                top: 20%;
            }

            .tile .monitor.large .middle {
                font-size: 2em;
            }

            .pointer {
                cursor: pointer;
            }

        </style>
        <script>

            {{ common_js }}

            // USER INTERFACE (INPUTS)

            function onTemperatureSettingsClick() {
                var temperature = prompt('Please enter a temperature value:');
                if (temperature >= 35 && temperature <= 40) apiSetCelsius(temperature, function () {
                    localStorage.setItem('temperature', temperature);
                    localStorage.setItem('unit', 'celsius');
                    setTemperatureView(temperature);
                    setUnitView('&#176;C');
                });
                else if (temperature >= 95 && temperature <= 104) apiSetFahrenheit(temperature, function () {
                    localStorage.setItem('temperature', temperature);
                    localStorage.setItem('unit', 'fahrenheit');
                    setTemperatureView(temperature);
                    setUnitView('&#176;F');
                });
                else alert('Invalid value should be in between 35 and 45 Celsius or 95 and 104 Fahrenheit');
            }


            function onStopBtnClick() {
                hide('panel');
                show('preloader');
                apiStop(reload, reload);
            }

            function onColourChangeClick() {
                apiColourChange();
            }

            function onTimerSettingsClick() {
                var timer = prompt('Please enter a timer value in minutes:');
                apiSetTimer(timer);
            }

            function onDocumentLoad() {
                
                setInterval(function() {
                    apiGetData(function(resp) {
                        r = JSON.parse(resp.responseText);
                        setOxygenView(r.oxygen);
                        setCelsusView(r.celsius);
                        setFahrenheitView(r.fahrenheit);
                        setRemainingView(r.remaining);
                    });
                }, {{ APP_DATA_REFRESH_PERIOD }});


                // set default temperature and unit to user preset

                var unit = localStorage.getItem('unit');
                var temperature = localStorage.getItem('temperature');
                if (unit == 'celsius') {
                    apiSetCelsius(temperature, function () {
                        setTemperatureView(temperature);
                        setUnitView('&#176;C');
                    });
                    setTemperatureView(temperature);
                    setUnitView('&#176;C');
                } else if (unit == 'fahrenheit') {
                    apiSetFahrenheit(temperature, function () {
                        setTemperatureView(temperature);
                        setUnitView('&#176;F');
                    });
                    setTemperatureView(temperature);
                    setUnitView('&#176;F');
                } else {
                    console.warn('temperature unit is unknown: ', unit);
                }
            }

            // USER INTERFACE (VIEWS)

            function setOxygenView(oxygen) {
                document.getElementById('oxygen').innerHTML = oxygen;
            }

            function setCelsusView(celsius) {
                document.getElementById('celsius').innerHTML = celsius;
            }

            function setFahrenheitView(fahrenheit) {
                document.getElementById('fahrenheit').innerHTML = fahrenheit;
            }

            function setRemainingView(remaining) {
                document.getElementById('remaining').innerHTML = remaining;
            }

            function setTemperatureView(temperature) {
                document.getElementById('temperature').innerHTML = temperature;
            }

            function setUnitView(unit) {
                document.getElementById('unit').innerHTML = unit;
            }

            // COMMUNICATION

            function apiSetCelsius(celsius, success, failure) {
                ajax("GET", "/set-celsius", {
                    'celsius': celsius
                }, success, failure);
            }

            function apiSetFahrenheit(fahrenheit, success, failure) {
                ajax("GET", "/set-fahrenheit", {
                    'fahrenheit': fahrenheit
                }, success, failure);
            }

            function apiColourChange(success, failure) {
                ajax("GET", "/change-colour", {}, success, failure);
            }

            function apiGetData(success, failure) {
                ajax("GET", "/get-data", {}, success, failure);
            }

            function apiStop(success, failure) {
                ajax("GET", "/stop", {}, success, failure);
            }

            function apiSetTimer(timer, success, failure) {
                ajax("GET", "/set-timer", {
                    'timer': timer
                }, success, failure);
            }
            
        </script>
    </head>
    <body onload="onDocumentLoad()">
        {{ common_html }}
        
        <div id="panel">
            <div class="tile pointer" onclick="onTemperatureSettingsClick()">
                <h1>TEMPERATURE SETTINGS</h1>
                <div class="vertical temperature">
                    <div class="top">
                        <span>40</span>&#176;C<br>
                        <span>104</span>&#176;F
                    </div>
                    <div class="middle">
                        <span id="temperature">...</span><span id="unit">...</span>
                    </div>
                    <div class="bottom">
                        <span>35</span>&#176;C<br>
                        <span>95</span>&#176;F
                    </div>
                </div>
            </div>

            <div class="tile">
                <h1>OXYGEN LEVEL</h1>
                <div class="vertical oxygen">
                    <div class="top">
                        <span>200</span>%
                    </div>
                    <div class="middle">
                        <span id="oxygen">...</span>%
                    </div>
                    <div class="bottom">
                        <span>50</span>%
                    </div>
                </div>
            </div>

            <div class="tile">
                <h1>WATER TEMPERATURE</h1>
                <div class="monitor">
                    <div class="middle">
                        <span id="celsius">...</span>&#176;C<br>
                        <span id="fahrenheit">...</span>&#176;F
                    </div>
                </div>
            </div>

            <div class="tile pointer" onclick="onTimerSettingsClick()">
                <h1>REMAINING UNTIL THE BATH</h1>
                <div class="monitor large">
                    <div class="middle">
                        <span id="remaining">...</span>
                    </div>
                </div>
            </div>

            <div class="tile pointer" onclick="onColourChangeClick()">
                <h1>COLOUR CHANGE</h1>
                <div class="circle colour">
                </div>
            </div>

            <div class="tile pointer" onclick="onStopBtnClick()">
                <h1>SYSTEM SHUTDOWN</h1>
                <div class="circle stop">
                    <div class="inner">  
                        <div class="middle">                  
                            <span>EMERGENCY STOP</span>
                        </div>
                    </div>
                </div>
            </div>
        </div>

    </body>
</html>
)PANEL_HTML";