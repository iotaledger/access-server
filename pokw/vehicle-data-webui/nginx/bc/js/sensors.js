//  Start Google map

// When the window has finished loading create our google map below
$(document).ready(function() {
    var map;
    var marker;
    var last_timestamp = 0;
    if(document.getElementById("map")){
        google.maps.event.addDomListener(window, 'load', initMap);

        // Initialize and add the map
        function initMap() {
          var init_loc = new google.maps.LatLng(45.257, 19.846);
          map = new google.maps.Map(
              document.getElementById('map'), {zoom: 16, center: init_loc});
          marker = new google.maps.Marker({position: init_loc, map: map});

        }

        function gotoMap(latitude, longitude){
            var coords = new google.maps.LatLng(latitude, longitude);
            marker.setPosition(coords);
            map.setCenter(coords);
        }
    }

    var ip_addr = document.location.href.split("//")[1].split(":")[0];
    console.log(ip_addr);
    var ip_port = "8882";
    var global_lat = 40.0;
    var global_lng = 40.0;

    var descriptions = {
        // Jaguar signals
        "odometer": {
            "desc": "An odometer or odograph is an instrument used for measuring the distance travelled by a vehicle.",
            "pretty_name": "Odometer [km]",
            "linearicon": "lnr-earth"
        },
        "rangekm": {
            "desc": "Driving range of a vehicle using only power from its electric battery pack to traverse a given driving cycle",
            "pretty_name": "Range [km]",
            "linearicon": "lnr-earth"
        },
        "gaspedal": {
            "desc": "The percentage of drivers having pressed the gas pedal for more than 10% during moving time windows of one second are depicted.",
            "pretty_name": "Gas pedal [%]",
            "linearicon": "lnr-rocket"
        },
        "enginespeed": {
            "desc": "Engine speed is calculated in terms of RPM or Revolutions per minute. Normally when you are driving your vehicle in a lower gear you are getting lower RPM",
            "pretty_name": "Engine speed [rpm]",
            "linearicon": "lnr-heart-pulse"
        },
        "estimatedchargingpower": {
            "desc": "Predicted powercharging demand of the electric vehicles",
            "pretty_name": "Charging power",
            "linearicon": "lnr-leaf"
        },
        "estimateddischargingpower": {
            "desc": "Predicted discharging output of the electric vehicles",
            "pretty_name": "Discharging power",
            "linearicon": "lnr-paw"
        },
        "distancetoempty": {
            "desc": "Predicted driving distance until the battery is empty",
            "pretty_name": "Distance to empty [km]",
            "linearicon": "lnr-earth"
        },
        "tyrepressure": {
            "desc": "Pressure for each one of the tyres on the vehicle",
            "pretty_name": "Tyre pressure",
            "linearicon": "lnr-dinner"
        },

        // CEVT signals
        "ambient_air_temperature": {
            "desc": "Ambient air temperature",
            "pretty_name": "Temperature",
            "linearicon": "lnr-rocket",
        },
        "fuel_tank_level": {
            "desc": "Fuel tank level",
            "pretty_name": "Fuel tank level",
            "linearicon": "lnr-dinner"
        },
        "central_locking_status_for_user_feedback": {
            "desc": "Central locking status for user feedback",
            "pretty_name": "Central locking status",
            "linearicon": "lnr-paw"
        },
        "trunk_status": {
            "desc": "Trunk status",
            "pretty_name": "Trunk status",
            "linearicon": "lnr-leaf"
        },
        "driver_door_status": {
            "desc": "Driver door status",
            "pretty_name": "Driver door status",
            "linearicon": "lnr-heart-pulse"
        },
        "driver_door_rear_status": {
            "desc": "Driver rear door status",
            "pretty_name": "Driver rear door status",
            "linearicon": "lnr-heart-pulse"
        },
        "passenger_door_status": {
            "desc": "Passenger door status",
            "pretty_name": "Passenger door status",
            "linearicon": "lnr-heart-pulse"
        },
        "passenger_door_rear_status": {
            "desc": "Passenger rear door status",
            "pretty_name": "Passenger rear door status",
            "linearicon": "lnr-heart-pulse"
        },
        "requested_brake_torque_at_wheels": {
            "desc": "Driver requested torque at wheels",
            "pretty_name": "Driver requested torque at wheels",
            "linearicon": "lnr-rocket"
        },
        "requested_propulsion_torque": {
            "desc": "Driver requested engine torque",
            "pretty_name": "Driver requested engine torque",
            "linearicon": "lnr-rocket"
        },
        "clutch_pedal_position": {
            "desc": "Clutch pedal position",
            "pretty_name": "Clutch pedal position",
            "linearicon": "lnr-rocket"
        },
        "brake_pedal_pressed": {
            "desc": "Brake pedal pressed",
            "pretty_name": "Brake pedal pressed",
            "linearicon": "lnr-rocket"
        }
    };

    function generate_sensor_display(item){
        var sensor_type = item.name;
        var value = item.value;
        var retElm = document.createElement("div");
        retElm.classList.add("single-service");
        var hdr = document.createElement("h4");
        var hdr5 = document.createElement("p");
        var desc = document.createElement("p");
        var iconSpan = document.createElement("span");
        iconSpan.classList.add("lnr");
        hdr.appendChild(iconSpan);
        if (sensor_type in descriptions) {
            iconSpan.classList.add(descriptions[sensor_type]["linearicon"]);
            if (typeof value === 'object') {
                hdr.appendChild(document.createTextNode(descriptions[sensor_type]["pretty_name"] + ":"));
                for (var key in value) {
                    hdr5.innerHTML += "<br>" + key + ": " + value[key];
                }
            } else {
                hdr.appendChild(document.createTextNode(descriptions[sensor_type]["pretty_name"] + ": " + value));
            }
            desc.textContent = descriptions[sensor_type]["desc"];
        } else {
            iconSpan.classList.add('lnr-dinner');
            if (typeof value === 'object') {
                var title_text = sensor_type;
                if ('unit' in value) {
                    title_text += (" [" + value['unit'] + "]");
                }
                title_text += ": ";
                if ('value' in value) {
                    title_text += value['value'];
                }
                hdr.appendChild(document.createTextNode(title_text));
                for (var key in value) {
                    if (key == 'value' || key == 'unit') continue;
                    hdr5.innerHTML += "<br>" + key + ": " + value[key];
                }
            } else {
                hdr.appendChild(document.createTextNode(sensor_type + ": " + value));
            }
            desc.textContent = "No description for " + sensor_type;
        }
        retElm.appendChild(hdr);
        if (typeof value === 'object') {
            retElm.appendChild(hdr5);
        }
        retElm.appendChild(desc);

        return retElm;
    }

    setInterval(function(){
        var xhr = new XMLHttpRequest();

        xhr.onload = function() {
            var service_container_container = document.getElementById("service");
            service_container_container.innerHTML = "";
            var service_container = document.createElement("div");
            service_container.classList.add("container");
            service_container_container.appendChild(service_container);

            var shared_vals = document.createElement("div")
            shared_vals.classList.add("row");
            shared_vals.classList.add("d-flex");
            shared_vals.classList.add("justify-content-center");
            service_container.appendChild(shared_vals);

            var shared_vals_subdiv = document.createElement("div");
            shared_vals_subdiv.classList.add("col-md-8");
            shared_vals_subdiv.classList.add("pb-40");
            shared_vals_subdiv.classList.add("header-text");
            shared_vals_subdiv.innerHTML="<h1>Shared sensor values of the vehicle</h1><p>Values are stored on IPFS server and shared via Whisper protocol</p>";
            shared_vals.appendChild(shared_vals_subdiv);

            var elm = null;
            if (xhr.status >= 200 && xhr.status < 300) {
                var json_data = JSON.parse(xhr.responseText);
                if ("error" in json_data) return;

                var rowElm = document.createElement("div");
                rowElm.classList.add("row");
                service_container.appendChild(rowElm);

                if (last_timestamp != json_data["timestamp"]) {
                    last_timestamp = json_data["timestamp"];
                    var json_terminal = document.getElementById("json_terminal");
                    json_terminal.innerHTML += JSON.stringify(json_data, null, 2);;
                    if (json_terminal.innerHTML.length > 4000) {
                        json_terminal.innerHTML = json_terminal.innerHTML.substring(json_terminal.innerHTML.length - 4000, json_terminal.innerHTML.length);
                    }
                    json_terminal.scrollTop = json_terminal.scrollHeight;
                }

                try {
                    for (var sensor_group in json_data) {
                        if (typeof json_data[sensor_group] === 'object' && json_data[sensor_group].constructor !== Array) continue;
                        if (sensor_group == "timestamp") continue;
                        json_data[sensor_group].forEach(function(item, index){
                            if (item.name == "location" && "value" in item) {
                                var lat = item.value["latitude"];
                                var lng = item.value["longitude"];
                                gotoMap(lat, lng);
                            } else if (item.name != "gps_data"){
                                var newElm = document.createElement("div");
                                newElm.classList.add("col-lg-4");
                                newElm.classList.add("col-md-6");
                                rowElm.appendChild(newElm);
                                newElm.appendChild(generate_sensor_display(item));
                            } else {
                                var lat = item.value["latitude"];
                                var lng = item.value["longitude"];
                                gotoMap(lat, lng);
                            }
                        });
                    }
                } catch (err) {
                    console.log('Unable to parse json for vehicle data');
                }

                if (last_timestamp != json_data["timestamp"]) {
                    last_timestamp = json_data["timestamp"];
                    var json_terminal = document.getElementById("json_terminal");
                    json_terminal.innerHTML += JSON.stringify(json_data, null, 2);;
                    if (json_terminal.innerHTML.length > 4000) {
                        json_terminal.innerHTML = json_terminal.innerHTML.substring(json_terminal.innerHTML.length - 4000, json_terminal.innerHTML.length);
                    }
                    json_terminal.scrollTop = json_terminal.scrollHeight;
                }
            }
        }

        let query = getJsonFromUrl(document.location.href);
        if ('deviceId' in query) {
            xhr.open('GET', "http://" + ip_addr + ":" + ip_port + "/cat_last/?deviceId=" + query.deviceId);
        } else {
            xhr.open('GET', "http://" + ip_addr + ":" + ip_port + "/cat_last/");
        }
        
        xhr.withCredentials = true;
        xhr.setRequestHeader('Content-Type', 'application/json');
        xhr.send();
    }, 1000);

    function getJsonFromUrl(url) {
        if(!url) url = location.href;
        var question = url.indexOf("?");
        var hash = url.indexOf("#");
        if(hash==-1 && question==-1) return {};
        if(hash==-1) hash = url.length;
        var query = question==-1 || hash==question+1 ? url.substring(hash) : 
        url.substring(question+1,hash);
        var result = {};
        query.split("&").forEach(function(part) {
          if(!part) return;
          part = part.split("+").join(" "); // replace every + with space, regexp-free version
          var eq = part.indexOf("=");
          var key = eq>-1 ? part.substr(0,eq) : part;
          var val = eq>-1 ? decodeURIComponent(part.substr(eq+1)) : "";
          var from = key.indexOf("[");
          if(from==-1) result[decodeURIComponent(key)] = val;
          else {
            var to = key.indexOf("]",from);
            var index = decodeURIComponent(key.substring(from+1,to));
            key = decodeURIComponent(key.substring(0,from));
            if(!result[key]) result[key] = [];
            if(!index) result[key].push(val);
            else result[key][index] = val;
          }
        });
        return result;
      }
});
