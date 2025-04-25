// Configuration
const WARNING_THRESHOLD = 30;
const DANGER_THRESHOLD = 50;
const API_URL = "flood.php?action=get_data"; // Endpoint to fetch data

// Initialize Chart
const ctx = document.getElementById('waterLevelChart').getContext('2d');
const chart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: [],
        datasets: [{
            label: 'Water Level (cm)',
            data: [],
            borderColor: '#3498db',
            backgroundColor: 'rgba(52, 152, 219, 0.1)',
            borderWidth: 3,
            tension: 0.2,
            pointRadius: 4,
            pointBackgroundColor: '#2980b9',
            fill: true
        }]
    },
    options: {
        responsive: true,
        interaction: {
            intersect: false,
            mode: 'index'
        },
        scales: {
            y: {
                beginAtZero: true,
                title: {
                    display: true,
                    text: 'Water Level (cm)',
                    font: {
                        weight: 'bold'
                    }
                },
                grid: {
                    color: 'rgba(0, 0, 0, 0.05)'
                }
            },
            x: {
                title: {
                    display: true,
                    text: 'Time',
                    font: {
                        weight: 'bold'
                    }
                },
                grid: {
                    display: false
                }
            }
        },
        plugins: {
            legend: {
                display: false
            },
            tooltip: {
                backgroundColor: '#2c3e50',
                titleFont: {
                    size: 14
                },
                bodyFont: {
                    size: 14
                }
            }
        }
    }
});

// Update Dashboard with Live Data
async function updateDashboard() {
    try {
        const response = await fetch(API_URL);
        const data = await response.json();
        
        // Update Current Level
        document.getElementById('currentLevel').textContent = data.current_level.toFixed(1);
        
        // Update Status
        updateStatus(data.current_level);
        
        // Update Last Reading Time
        updateTimestamp(data.timestamp);
        
        // Update Chart
        updateChart(data.history);
        
        // Update Alerts
        if (data.current_level >= WARNING_THRESHOLD) {
            createAlert(data.current_level, data.timestamp);
        }
        
    } catch (error) {
        console.error("Error fetching data:", error);
        document.getElementById('currentLevel').textContent = "ERR";
    }
}

// Update Status Card
function updateStatus(level) {
    const statusText = document.getElementById('statusText');
    const statusDetail = document.getElementById('statusDetail');
    
    if (level >= DANGER_THRESHOLD) {
        statusText.innerHTML = '<span class="status-badge bg-danger alert-pulse">DANGER</span>';
        statusDetail.textContent = 'Flood risk! Immediate action required';
        statusDetail.className = 'text-danger';
    } else if (level >= WARNING_THRESHOLD) {
        statusText.innerHTML = '<span class="status-badge bg-warning">WARNING</span>';
        statusDetail.textContent = 'Water level is rising';
        statusDetail.className = 'text-warning';
    } else {
        statusText.innerHTML = '<span class="status-badge bg-success">NORMAL</span>';
        statusDetail.textContent = 'No flood detected';
        statusDetail.className = 'text-success';
    }
}

// Update Timestamp
function updateTimestamp(timestamp) {
    const dateObj = new Date(timestamp);
    document.getElementById('lastUpdateTime').textContent = 
        dateObj.toLocaleTimeString([], { hour: '2-digit', minute: '2-digit', second: '2-digit' });
    document.getElementById('lastUpdateDate').textContent = 
        dateObj.toLocaleDateString([], { weekday: 'short', month: 'short', day: 'numeric' });
}

// Update Chart
function updateChart(historyData) {
    chart.data.labels = historyData.map(item => 
        new Date(item.timestamp).toLocaleTimeString([], { hour: '2-digit', minute: '2-digit' })
    );
    chart.data.datasets[0].data = historyData.map(item => item.water_level);
    chart.update();
}

// Create Alert
function createAlert(level, timestamp) {
    const alertContainer = document.getElementById('alertContainer');
    const noAlertsMessage = document.getElementById('noAlertsMessage');
    
    noAlertsMessage.style.display = 'none';
    
    const alertTime = new Date(timestamp).toLocaleTimeString();
    const alertLevel = level.toFixed(1);
    
    const alertHTML = `
        <div class="alert alert-danger d-flex align-items-center mb-3">
            <i class="fas fa-exclamation-triangle fa-2x me-3"></i>
            <div>
                <strong>Flood Warning!</strong> Water level reached ${alertLevel} cm
                <div class="text-muted small mt-1"><i class="fas fa-clock me-1"></i>${alertTime}</div>
            </div>
        </div>
    `;
    
    alertContainer.insertAdjacentHTML('afterbegin', alertHTML);
}

// Auto-refresh every 3 seconds
setInterval(updateDashboard, 3000);
updateDashboard(); // Initial load