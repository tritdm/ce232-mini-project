$(document).ready(function(){
    var _temp_values;
    var _temp_times;
    $.ajax({
        url: "/get_data",
        type: "get",
        data: {vals: ''},
        success: function(response) {
            full_data = JSON.parse(response.payload);
            _temp_values = full_data['temp_values'];
            _temp_times = full_data['temp_times'];
        },

    });
    new Chart(document.getElementById("lineChartTemp"), {
        type: 'line',
        data: {
            labels: { temp_times | safe },
            datasets: [
                {
                    label: "Temperature",
                    backgroundColor: ["#3e95cd", "#8e5ea2","#3cba9f","#e8c3b9","#c45850"],
                    data: {_temp_values | safe }
                }
            ]
            },
                options: {
                    legend: { display: false },
                        title: {
                        display: true,
                        text: 'Temperature'
                    }
                }
    });
});