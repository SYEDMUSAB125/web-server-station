const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
  <head>
    <title>ESP32 LORA RA-02 WEB SERVER</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
    <link rel="icon" href="data:,">
    <style>
      html {font-family: Arial; display: inline-block; text-align: center;}
      p {font-size: 1.2rem;}
      h4 {font-size: 0.8rem;}
      body {margin: 0;}
      .topnav {overflow: hidden; background-color: #00878F; color: white; font-size: 1.2rem;}
      .content {padding: 5px; }
      .card {background-color: white; box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5); border: 1px solid #00878f; border-radius: 15px;}
      .card.header {background-color: #00878f; color: white; border-bottom-right-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 12px; border-top-left-radius: 12px;}
      .cards {max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));}
      .reading {font-size: 1.3rem;}
      .packet {color: #bebebe;}
      .temperatureColor {color: #fd7e14;}
      .humidityColor {color: #1b78e2;}
      .statusreadColor {color: #702963; font-size:12px;}
      .LEDColor {color: #183153;}
      
      /* ----------------------------------- Toggle Switch */
      .switch {
        position: relative;
        display: inline-block;
        width: 50px;
        height: 24px;
      }

      .switch input {display:none;}

      .sliderTS {
        position: absolute;
        cursor: pointer;
        top: 0;
        left: 0;
        right: 0;
        bottom: 0;
        background-color: #D3D3D3;
        -webkit-transition: .4s;
        transition: .4s;
        border-radius: 34px;
      }

      .sliderTS:before {
        position: absolute;
        content: "";
        height: 16px;
        width: 16px;
        left: 4px;
        bottom: 4px;
        background-color: #f7f7f7;
        -webkit-transition: .4s;
        transition: .4s;
        border-radius: 50%;
      }

      input:checked + .sliderTS {
        background-color: #00878F;
      }

      input:focus + .sliderTS {
        box-shadow: 0 0 1px #2196F3;
      }

      input:checked + .sliderTS:before {
        -webkit-transform: translateX(26px);
        -ms-transform: translateX(26px);
        transform: translateX(26px);
      }

      .sliderTS:after {
        content:'OFF';
        color: white;
        display: block;
        position: absolute;
        transform: translate(-50%,-50%);
        top: 50%;
        left: 70%;
        font-size: 10px;
        font-family: Verdana, sans-serif;
      }

      input:checked + .sliderTS:after {  
        left: 25%;
        content:'ON';
      }

      input:disabled + .sliderTS {  
        opacity: 0.3;
        cursor: not-allowed;
        pointer-events: none;
      }
      /* ----------------------------------- */
      
      /* ----------------------------------- Tooltip */
      .tooltip {
        position: relative;
        display: inline-block;
      }

      .tooltip .tooltiptext {
        visibility: hidden;
        width: 120px;
        background-color: #00878F;
        color: #fff;
        text-align: center;
        border-radius: 6px;
        padding: 5px 0;
        position: absolute;
        z-index: 1;
        bottom: 125%;
        left: 50%;
        margin-left: -60px;
        opacity: 0;
        /*transition: opacity 0.3s;*/
        transition: visibility 0s linear 300ms, opacity 300ms;
      }

      .tooltip .tooltiptext::after {
        content: "";
        position: absolute;
        top: 100%;
        left: 50%;
        margin-left: -5px;
        border-width: 5px;
        border-style: solid;
        border-color: #00878F transparent transparent transparent;
      }

      .tooltip:hover .tooltiptext {
        visibility: visible;
        animation: tooltipkeys 3s 1;
        opacity: 1;
      }
      @-webkit-keyframes tooltipkeys{ 0%   {opacity: 0;} 75%  {opacity: 0;} 100% {opacity: 1;} }
      @-moz-keyframes tooltipkeys{ 0%   {opacity: 0;} 75%  {opacity: 0;} 100% {opacity: 1;} }
      @-o-keyframes tooltipkeys{ 0%   {opacity: 0;} 75%  {opacity: 0;} 100% {opacity: 1;} }
      @keyframes tooltipkeys { 0%   {opacity: 0;} 75%  {opacity: 0;} 100% {opacity: 1;} }
      /* ----------------------------------- */
    </style>
  </head>
  
  <body>
    <div class="topnav">
      <h3>ESP32 LORA RA-02 WEB SERVER</h3>
    </div>
    
    <br>
    
    <!-- __ DISPLAYS MONITORING AND CONTROLLING ____________________________________________________________________________________________ -->
    <div class="content">
      <div class="cards">
        
        <!-- == ESP32 Slave 1 Monitoring And Controlling =========================================================== -->
        <div class="card">
          <div class="card header">
            <h3 style="font-size: 1.2rem;">ESP32 SLAVE 1</h3>
          </div>
          
          <!-- Displays the humidity and temperature values received from Slave 1. *** -->
          <h4 class="temperatureColor"><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4>
          <p class="temperatureColor"><span class="reading"><span id="temp_Slave_1"></span> &deg;C</span></p>
          <h4 class="humidityColor"><i class="fas fa-tint"></i> HUMIDITY</h4>
          <p class="humidityColor"><span class="reading"><span id="humd_Slave_1"></span> &percnt;</span></p>
          <!-- *********************************************************************** -->
          
          <!-- Buttons for controlling the LEDs on Slave 1. ************************** -->
          <div style="position:relative;">
            <div style="position:relative; float: left;top: 3px;width: 47%;left: 0px;text-align: right;">
              <span class="LEDColor" style="font-weight:bold;"><i class="fas fa-lightbulb"></i> LED 1 : </span>
            </div>
            <div style="position:relative; float: right;width: 51%; right: 0px;">
              <div style="position:relative; float: left;">
                <label class="switch">
                  <input type="checkbox" id="togLED_1_Slave_1" onclick="send_LED_State('togLED_1_Slave_1','S1','1')">
                  <div class="sliderTS"></div>
                </label>
              </div>
              <div class="tooltip" style="float: left; margin-left: 8px;">
                <span class="tooltiptext">Invert the toggle button state.</span>
                <button type="button" onclick="change_TglBtn_Checked('togLED_1_Slave_1')" style="height: 24px; line-height: 24px; font-size: 13px; display: inline-flex; align-items: center;">&#8644;</button>
              </div>
            </div>
            
            <div style="position:relative; float: left;top: 18px;width: 47%;left: 0px; text-align: right;">
              <span class="LEDColor" style="font-weight:bold;"><i class="fas fa-lightbulb"></i> LED 2 : </span>
            </div>
            <div style="position:relative;float: right; width: 51%;right: 0px; top: 5px;">
              <div style="position:relative; float: left;">
                <label class="switch">
                  <input type="checkbox" id="togLED_2_Slave_1" onclick="send_LED_State('togLED_2_Slave_1','S1','2')">
                  <div class="sliderTS"></div>
                </label>
              </div>
              <div class="tooltip" style="float: left; margin-left: 8px;">
                <span class="tooltiptext">Invert the toggle button state.</span>
                <button type="button" onclick="change_TglBtn_Checked('togLED_2_Slave_1')" style="height: 24px; line-height: 24px; font-size: 13px; display: inline-flex; align-items: center;">&#8644;</button>
              </div>
            </div>
          </div>
          <!-- *********************************************************************** -->
          
          <br><br><br><br>
          
          <!-- Shows other information received from Slave 1. ************************ -->
          <p class="statusreadColor"><b><span>Last time to receive data : </span><span id="LTRD_Slave_1"></span></b></p>
          <p class="statusreadColor"><span>Status Read Sensor DHT11 : </span><span id="status_read_DHT11_Slave_1"></span></p>
          <p class="statusreadColor"><span>LED 1 State : </span><span id="LED_1_State_Slave_1"></span></p>
          <p class="statusreadColor"><span>LED 2 State : </span><span id="LED_2_State_Slave_1"></span></p>
          <!-- *********************************************************************** -->
        </div>
        <!-- ======================================================================================================= -->
        
        <!-- == ESP32 Slave 2 Monitoring And Controlling =========================================================== -->
        <div class="card">
          <div class="card header">
            <h3 style="font-size: 1.2rem;">ESP32 SLAVE 2</h3>
          </div>
          
          <!-- Displays the humidity and temperature values received from Slave 2. *** -->
          <h4 class="temperatureColor"><i class="fas fa-thermometer-half"></i> TEMPERATURE</h4>
          <p class="temperatureColor"><span class="reading"><span id="temp_Slave_2"></span> &deg;C</span></p>
          <h4 class="humidityColor"><i class="fas fa-tint"></i> HUMIDITY</h4>
          <p class="humidityColor"><span class="reading"><span id="humd_Slave_2"></span> &percnt;</span></p>
          <!-- *********************************************************************** -->
          
          <!-- Buttons for controlling the LEDs on Slave 2. ************************** -->
          <div style="position:relative;">
            <div style="position:relative; float: left;top: 3px;width: 47%; left: 0px;text-align: right;">
              <span class="LEDColor" style="font-weight:bold;"><i class="fas fa-lightbulb"></i> LED 1 : </span>
            </div>
            <div style="position:relative; float: right; width: 51%; right: 0px;">
              <div style="position:relative; float: left;">
                <label class="switch">
                  <input type="checkbox" id="togLED_1_Slave_2" onclick="send_LED_State('togLED_1_Slave_2','S2','1')">
                  <div class="sliderTS"></div>
                </label>
              </div>
              <div class="tooltip" style="float: left; margin-left: 8px;">
                <span class="tooltiptext">Invert the toggle button state.</span>
                <button type="button" onclick="change_TglBtn_Checked('togLED_1_Slave_2')" style="height: 24px; line-height: 24px; font-size: 13px; display: inline-flex; align-items: center;">&#8644;</button>
              </div>
            </div>
            
            <div style="position:relative;float: left;top: 18px;width: 47%;left: 0px; text-align: right;">
              <span class="LEDColor" style="font-weight:bold;"><i class="fas fa-lightbulb"></i> LED 2 : </span>
            </div>
            <div style="position:relative; float: right;width: 51%;right: 0px;top: 5px;">
              <div style="position:relative;float: left;">
                <label class="switch">
                  <input type="checkbox" id="togLED_2_Slave_2" onclick="send_LED_State('togLED_2_Slave_2','S2','2')">
                  <div class="sliderTS"></div>
                </label>
              </div>
              <div class="tooltip" style="float: left; margin-left: 8px;">
                <span class="tooltiptext">Invert the toggle button state.</span>
                <button type="button" onclick="change_TglBtn_Checked('togLED_2_Slave_2')" style="height: 24px; line-height: 24px; font-size: 13px; display: inline-flex; align-items: center;">&#8644;</button>
              </div>
            </div>
          </div>
          <!-- *********************************************************************** -->
          
          <br><br><br><br>
          
          <!-- Shows other information received from Slave 2. ************************ -->
          <p class="statusreadColor"><b><span>Last time to receive data : </span><span id="LTRD_Slave_2"></span></b></p>
          <p class="statusreadColor"><span>Status Read Sensor DHT11 : </span><span id="status_read_DHT11_Slave_2"></span></p>
          <p class="statusreadColor"><span>LED 1 State : </span><span id="LED_1_State_Slave_2"></span></p>
          <p class="statusreadColor"><span>LED 2 State : </span><span id="LED_2_State_Slave_2"></span></p>
          <!-- *********************************************************************** -->
        </div>  
        <!-- ======================================================================================================= -->
        
      </div>
    </div>
    <!-- ___________________________________________________________________________________________________________________________________ -->
    
    <p id="Show_Info" style="font-size: 0.9rem; color: #00878F;"></p>
    
    <script>
      //------------------------------------------------------------
      var Timer = setInterval(myTimer, 100);
      var TimerShowInfo = setInterval(myTimerShowInfo, 100);
      var count_to_Ctrl_TglBtn = 0;
      var count_to_Show_Info = 0;
      var count_to_Show_Info_no_Data_is_coming = 0;
      let change_TglBtn_Disable = false;
      let start_Show_Info_For_First_Time = true;
      //------------------------------------------------------------
      
      //------------------------------------------------------------
      document.getElementById("temp_Slave_1").innerHTML = "NN"; 
      document.getElementById("humd_Slave_1").innerHTML = "NN";
      document.getElementById("status_read_DHT11_Slave_1").innerHTML = "NN";
      document.getElementById("LTRD_Slave_1").innerHTML = "NN";
      document.getElementById("LED_1_State_Slave_1").innerHTML = "NN";
      document.getElementById("LED_2_State_Slave_1").innerHTML = "NN";
      
      
      document.getElementById("temp_Slave_2").innerHTML = "NN"; 
      document.getElementById("humd_Slave_2").innerHTML = "NN";
      document.getElementById("status_read_DHT11_Slave_2").innerHTML = "NN";
      document.getElementById("LTRD_Slave_2").innerHTML = "NN";
      document.getElementById("LED_1_State_Slave_2").innerHTML = "NN";
      document.getElementById("LED_2_State_Slave_2").innerHTML = "NN";

      document.getElementById("Show_Info").innerHTML = "Please wait a while to receive data from Slaves.";
      //------------------------------------------------------------

      //------------------------------------------------------------ Processes the data received from the ESP32 Master.
      if (!!window.EventSource) {
        var source = new EventSource('/events');
        
        source.addEventListener('open', function(e) {
          console.log("Events Connected");
        }, false);
        
        source.addEventListener('error', function(e) {
          if (e.target.readyState != EventSource.OPEN) {
            console.log("Events Disconnected");
          }
        }, false);
        
        source.addEventListener('message', function(e) {
          console.log("message", e.data);
        }, false);

        source.addEventListener('allDataJSON', function(e) {
          console.log("allDataJSON", e.data);

          var today = new Date();
          var time = leading_zeros(today.getHours()) + ":" + leading_zeros(today.getMinutes()) + ":" + leading_zeros(today.getSeconds());

          count_to_Show_Info_no_Data_is_coming = 0;
          if (document.getElementById("Show_Info").innerHTML == "No data comes from all Slaves after 10 seconds from the last time the Master received data.") {
            document.getElementById("Show_Info").innerHTML = "";
          }
          
          var obj = JSON.parse(e.data);
          
          if (obj.ID_Slave == "S1") {
            document.getElementById("status_read_DHT11_Slave_1").innerHTML = obj.StatusReadDHT11;
            if (obj.StatusReadDHT11 == "FAILED") {
              document.getElementById("status_read_DHT11_Slave_1").style.color = "red";
            } else {
              document.getElementById("status_read_DHT11_Slave_1").style.color = "#702963";
            }
            
            document.getElementById("humd_Slave_1").innerHTML = obj.Humd;
            document.getElementById("temp_Slave_1").innerHTML = obj.Temp.toFixed(2);
            
            if (obj.LED1 == true) {
              document.getElementById("LED_1_State_Slave_1").innerHTML = "ON";
            } else if (obj.LED1 == false) {
              document.getElementById("LED_1_State_Slave_1").innerHTML = "OFF";
            } else {
              document.getElementById("LED_1_State_Slave_1").innerHTML = "NN";
            }
            
            if (obj.LED2 == true) {
              document.getElementById("LED_2_State_Slave_1").innerHTML = "ON";
            } else if (obj.LED2 == false) {
              document.getElementById("LED_2_State_Slave_1").innerHTML = "OFF";
            } else {
              document.getElementById("LED_2_State_Slave_1").innerHTML = "NN";
            }
            
            document.getElementById("LTRD_Slave_1").innerHTML = time;
          }
          
          if (obj.ID_Slave == "S2") {
            document.getElementById("status_read_DHT11_Slave_2").innerHTML = obj.StatusReadDHT11;
            
            if (obj.StatusReadDHT11 == "FAILED") {
              document.getElementById("status_read_DHT11_Slave_2").style.color = "red";
            } else {
              document.getElementById("status_read_DHT11_Slave_2").style.color = "#702963";
            }
            
            document.getElementById("humd_Slave_2").innerHTML = obj.Humd;
            document.getElementById("temp_Slave_2").innerHTML = obj.Temp.toFixed(2);
            
            if (obj.LED1 == true) {
              document.getElementById("LED_1_State_Slave_2").innerHTML = "ON";
            } else if (obj.LED1 == false) {
              document.getElementById("LED_1_State_Slave_2").innerHTML = "OFF";
            } else {
              document.getElementById("LED_1_State_Slave_2").innerHTML = "NN";
            }
            
            if (obj.LED2 == true) {
              document.getElementById("LED_2_State_Slave_2").innerHTML = "ON";
            } else if (obj.LED2 == false) {
              document.getElementById("LED_2_State_Slave_2").innerHTML = "OFF";
            } else {
              document.getElementById("LED_2_State_Slave_2").innerHTML = "NN";
            }
            
            document.getElementById("LTRD_Slave_2").innerHTML = time;
          }
        }, false);
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------ Timer to deactivate the button a few moments after being clicked.
      function myTimer() {
        if (change_TglBtn_Disable == true) {
          count_to_Ctrl_TglBtn += 100;
          if (count_to_Ctrl_TglBtn > 1000) {
            count_to_Ctrl_TglBtn = 0;
            change_TglBtn_Disable = false;
            
            document.getElementById("togLED_1_Slave_1").disabled = false;
            document.getElementById("togLED_2_Slave_1").disabled = false;
            document.getElementById("togLED_1_Slave_2").disabled = false;
            document.getElementById("togLED_2_Slave_2").disabled = false; 
          }
        }
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------
      function myTimerShowInfo() {
        count_to_Show_Info_no_Data_is_coming += 100;
        if (count_to_Show_Info_no_Data_is_coming > 10000) {
          count_to_Show_Info_no_Data_is_coming = 0;

          document.getElementById("Show_Info").innerHTML = "No data comes from all Slaves after 10 seconds from the last time the Master received data.";
        }
        
        count_to_Show_Info += 100;
        if (count_to_Show_Info > 1500) {
          count_to_Show_Info = 0;

          if (start_Show_Info_For_First_Time == true) {
            start_Show_Info_For_First_Time = false;
            document.getElementById("Show_Info").innerHTML = "";
            return;
          }
        
          if (document.getElementById("Show_Info").innerHTML == "Sending Data to Slave 1, Please Wait...") {
            document.getElementById("Show_Info").innerHTML = "Please wait a moment for Slave 1 to reply to the message."
            return;
          }
          if (document.getElementById("Show_Info").innerHTML == "Please wait a moment for Slave 1 to reply to the message.") {
            document.getElementById("Show_Info").innerHTML = "";
            return;
          }
          
          if (document.getElementById("Show_Info").innerHTML == "Sending Data to Slave 2, Please Wait...") {
            document.getElementById("Show_Info").innerHTML = "Please wait a moment for Slave 2 to reply to the message."
            return;
          }
          if (document.getElementById("Show_Info").innerHTML == "Please wait a moment for Slave 2 to reply to the message.") {
            document.getElementById("Show_Info").innerHTML = "";
            return;
          }
        }
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------ 
      function change_TglBtn_Checked(TglBtnID) {
        var chck = document.getElementById(TglBtnID);
        chck.checked = !chck.checked;
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------
      function leading_zeros(x) { 
        return (x < 10 ? '0' : '') + x;
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------ The function called by "Toggle Switch" to control the LED.
      function send_LED_State(id,slave,led_num) {
        count_to_Show_Info = 0;
        change_TglBtn_Disable = true;
        if (slave == "S1") document.getElementById("Show_Info").innerHTML = "Sending Data to Slave 1, Please Wait...";
        if (slave == "S2") document.getElementById("Show_Info").innerHTML = "Sending Data to Slave 2, Please Wait...";
        
        document.getElementById("togLED_1_Slave_1").disabled = true;
        document.getElementById("togLED_2_Slave_1").disabled = true;
        document.getElementById("togLED_1_Slave_2").disabled = true;
        document.getElementById("togLED_2_Slave_2").disabled = true;  
        
        var tgLEDFlash = document.getElementById(id);
        var tgState;

        if (tgLEDFlash.checked == true) tgState = "t";
        if (tgLEDFlash.checked == false) tgState = "f";

        send_cmd(slave,led_num,tgState);
      }
      //------------------------------------------------------------
      
      //------------------------------------------------------------ XMLHttpRequest to submit data.
      function send_cmd(slave,led_num,value) {
        var xhr = new XMLHttpRequest();
        xhr.open("GET", "set_LED?Slave_Num="+slave+"&LED_Num="+led_num+"&LED_Val="+value, true);
        xhr.send();
      }
      //------------------------------------------------------------
    </script>
  </body>
</html>
)=====";



