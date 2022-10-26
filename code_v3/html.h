char page[] PROGMEM = R"(
<!DOCTYPE html>
<html lang='en'>
<head>
    <meta charset='UTF-8'>
    <meta http-equiv='X-UA-Compatible' content='IE=edge'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Midsem project</title>
    <style>
        html 
        { 
            font-family: Helvetica; 
            display: inline-block; 
            margin: 0px auto; 
            text-align: center;
        }
        body
        {
            margin-top: 50px;
        } 
        h1 
        {
            color: #444444;
            margin: 50px auto 30px;
        }
        p 
        {
            font-size: 24px;
            color: #444444;
            margin-bottom: 10px;
        }

        .toggle {
            --width: 90px;
            --height: calc(var(--width) / 3);

            position: relative;
            display: inline-block;
            width: var(--width);
            height: var(--height);
            border-radius: calc(var(--height/3));
            cursor: pointer;
        }

        .toggle input {
            display: none;
        }

        .toggle .slider {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            border-radius: calc(var(--height)/3);
            background-color: #ccc;
            transition: all 0.4s ease-in-out;
        }

        .toggle .slider::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            width: calc(var(--height));
            height: calc(var(--height));
            border-radius: calc(var(--height) / 3);
            background-color: #fff;
            box-shadow: 0px 1px 3px rgba(0, 0, 0, 0.3);
            transition: all 0.4s ease-in-out;
        }

        .toggle input:checked+.slider {
            background-color: #2196F3;
        }

        .toggle input:checked+.slider::before {
            transform: translateX(calc(var(--width) - var(--height)));
        }

        .toggle .labels {
            position: absolute;
            top: 6px;
            left: 0px;
            width: 100%;
            height: 100%;
            font-size: 16px;
            font-family: Helvetica;
            transition: all 0.4s ease-in-out;
        }

        .toggle .labels::after {
            content: attr(data-off);
            position: absolute;
            right: 5px;
            color: #4d4d4d;
            opacity: 1;
            transition: all 0.4s ease-in-out;
        }

        .toggle .labels::before {
            content: attr(data-on);
            position: absolute;
            left: 5px;
            color: #ffffff;
            opacity: 0;
            transition: all 0.4s ease-in-out;
        }

        .toggle input:checked~.labels::after {
            opacity: 0;
        }

        .toggle input:checked~.labels::before {
            opacity: 1;
        }
        .switch {
            display: flex;
            align-items: center;
            justify-content: center;
            margin-top: 1rem;
        }
                .btn {
            padding: 1rem;
            background-color: #2196F3;
            color: white;
            border-radius: 5px;
            outline: none;
            margin-right: 1rem;
            border-color: #2196F3;
        }

        .btn-red {
            background-color: #f32121;
            border-color:#f32121;
        }
        #buttons {
            visibility: hidden;
        }
    </style>
    <script>
        var automatic = true;
        setInterval(loadDoc,2000);
        function loadDoc() {
            var xhttp = new XMLHttpRequest();
            xhttp.open('GET', '/getDistance', true);
            xhttp.onreadystatechange = function() {
            if (this.readyState == 4 && this.status == 200) {
                    document.getElementById('waterValue').innerHTML = this.responseText;
                };
            };
            xhttp.send();
        }

        function changeOperationState(mode) {
            var xhttp = new XMLHttpRequest();
            xhttp.open('GET', '/'+mode, true);
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    // do nothing
                    // document.getElementById(webpage).innerHTML = this.responseText};
                };
            };
            xhttp.send();
        }

        function turnMotorOn() {
            var xhttp = new XMLHttpRequest();
            xhttp.open('GET', '/on', true);
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    // do nothing
                    // document.getElementById(webpage).innerHTML = this.responseText};
                };
            };
            xhttp.send();
        }

        function turnMotorOff() {
            var xhttp = new XMLHttpRequest();
            xhttp.open('GET', '/off', true);
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    // do nothing
                    // document.getElementById(webpage).innerHTML = this.responseText};
                }
            }
            xhttp.send();
        }

        function toggleState() {
            automatic = !automatic;
            mode = automatic ? 'auto' : 'manual';
            changeOperationState(mode);
            document.getElementById('buttons').style.visibility = automatic ? "hidden" : "visible";
        }
    </script>
</head>
<body>
    <div id='webpage'>
        <h1>MIDSEM project - Daniel and Isaac</h1>
        <p>Water level: <span id='waterValue'></span></p>
    </div>
    
    <div class='switch'>
        <span>Motor control: &emsp;</span> 
        <label class='toggle'>
            <input onclick='toggleState()' type='checkbox'>
            <span class='slider'></span>
            <span class='labels' data-on='Manual' data-off='Auto'></span>
        </label>
    </div>
    <div id='buttons' class='switch'>
        <button onclick='turnMotorOn()' class='btn'>Turn motor on</button>
        <button onclick='turnMotorOff()' class='btn btn-red'>Turn motor off</button>
    </div>
</body>
</html>
)";
