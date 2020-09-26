const char* common_css = R"COMMON_CSS(
    
button {
    background: #3D94F6;
    background-image: -webkit-linear-gradient(top, #3D94F6, #1E62D0);
    background-image: -moz-linear-gradient(top, #3D94F6, #1E62D0);
    background-image: -ms-linear-gradient(top, #3D94F6, #1E62D0);
    background-image: -o-linear-gradient(top, #3D94F6, #1E62D0);
    background-image: linear-gradient(to bottom, #3D94F6, #1E62D0);
    -webkit-border-radius: 20px;
    -moz-border-radius: 20px;
    border-radius: 20px;
    color: #FFFFFF;
    font-family: Open Sans;
    font-size: 40px;
    font-weight: 100;
    padding: 40px;
    box-shadow: 1px 1px 20px 0px #000000;
    -webkit-box-shadow: 1px 1px 20px 0px #000000;
    -moz-box-shadow: 1px 1px 20px 0px #000000;
    text-shadow: 1px 1px 20px #000000;
    border: solid #337FED 1px;
    text-decoration: none;
    display: inline-block;
    cursor: pointer;
    text-align: center;
}

button:hover {
    border: solid #337FED 1px;
    background: #1E62D0;
    background-image: -webkit-linear-gradient(top, #1E62D0, #3D94F6);
    background-image: -moz-linear-gradient(top, #1E62D0, #3D94F6);
    background-image: -ms-linear-gradient(top, #1E62D0, #3D94F6);
    background-image: -o-linear-gradient(top, #1E62D0, #3D94F6);
    background-image: linear-gradient(to bottom, #1E62D0, #3D94F6);
    -webkit-border-radius: 20px;
    -moz-border-radius: 20px;
    border-radius: 20px;
    text-decoration: none;
}

#preloader {
    margin: 0; 
    padding: 0; 
    border: 0;
    position: fixed;
    top: 0;
    left: 0;
    width: 100%;
    height: 100%;
    opacity: 0.5;
    display: none;
    text-align: center;
}

#message {
    margin: 0; 
    padding: 20px 40px;
    position: fixed;
    top: 30%;
    left: 50%;
    -ms-transform: translateX(-50%);
    transform: translateX(-50%);
    width: 80%;
    display: none;
    text-align: center;
    border: 1px solid black;
}
#message.success {
    background: lightgreen;
    color: black;
}

#message.error {
    background: red;
    color: white;
}

)COMMON_CSS";