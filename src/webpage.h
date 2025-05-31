// Web Server Functions
String html = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset="UTF-8">
  <title>Soil Monitoring System</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <script src="https://cdn.jsdelivr.net/npm/chart.js"></script>
  <style>
    :root {
      --primary: #4ade80;
      --secondary: #60a5fa;
      --accent: #f59e0b;
      --dark: #1e293b;
      --darker: #0f172a;
      --light: #f8fafc;
      --gray: #64748b;
    }
    
    body {
      font-family: 'Inter', -apple-system, BlinkMacSystemFont, sans-serif;
      margin: 0;
      padding: 0;
      background-color: var(--darker);
      color: var(--light);
      min-height: 100vh;
    }
    
    .container {
      max-width: 1400px;
      margin: 0 auto;
      padding: 20px;
    }
    
    header {
      background: linear-gradient(135deg, var(--dark), var(--darker));
      color: white;
      padding: 25px 0;
      text-align: center;
      border-radius: 12px;
      margin-bottom: 25px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      border: 1px solid rgba(255,255,255,0.05);
      position: relative;
      overflow: hidden;
    }
    
    header::after {
      content: '';
      position: absolute;
      top: 0;
      left: 0;
      right: 0;
      height: 4px;
      background: linear-gradient(90deg, var(--primary), var(--secondary), var(--accent));
    }
    
    h1 {
      margin: 0;
      font-size: 2.4em;
      font-weight: 700;
      background: linear-gradient(90deg, var(--primary), var(--secondary));
      -webkit-background-clip: text;
      background-clip: text;
      color: transparent;
    }
    
    .dashboard {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));
      gap: 25px;
      margin-bottom: 25px;
    }
    
    .card {
      background-color: var(--dark);
      border-radius: 12px;
      padding: 25px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      transition: all 0.3s ease;
      border: 1px solid rgba(255,255,255,0.05);
      position: relative;
    }
    
    .card:hover {
      transform: translateY(-5px);
      box-shadow: 0 10px 15px rgba(0,0,0,0.2);
      border-color: rgba(255,255,255,0.1);
    }
    
    .card h2 {
      color: var(--light);
      margin-top: 0;
      padding-bottom: 15px;
      font-weight: 600;
      font-size: 1.3em;
      display: flex;
      align-items: center;
      gap: 10px;
    }
    
    .card h2 svg {
      width: 24px;
      height: 24px;
    }
    
    .value {
      font-size: 3em;
      font-weight: 700;
      margin: 15px 0;
      background: linear-gradient(90deg, var(--primary), var(--secondary));
      -webkit-background-clip: text;
      background-clip: text;
      color: transparent;
    }
    
    .unit {
      font-size: 1em;
      color: var(--gray);
      margin-bottom: 15px;
    }
    
    .status {
      text-align: right;
      color: var(--gray);
      font-size: 0.9em;
      margin-top: 20px;
    }
    
    .map-container {
      height: 350px;
      background-color: var(--dark);
      border-radius: 12px;
      overflow: hidden;
      margin-top: 20px;
      border: 1px solid rgba(255,255,255,0.05);
    }
    
    .chart-container {
      height: 300px;
      margin-top: 20px;
      position: relative;
    }
    
    .gauge-container {
      display: flex;
      align-items: center;
      gap: 15px;
      margin: 20px 0;
    }
    
    .gauge-label {
      width: 80px;
      font-size: 0.9em;
      color: var(--gray);
    }
    
    .gauge {
      flex: 1;
      height: 12px;
      background-color: rgba(255,255,255,0.1);
      border-radius: 6px;
      overflow: hidden;
    }
    
    .gauge-fill {
      height: 100%;
      border-radius: 6px;
      transition: width 0.5s ease;
    }
    
    .gauge-fill.nitrogen {
      background: var(--primary);
    }
    
    .gauge-fill.phosphorus {
      background: var(--secondary);
    }
    
    .gauge-fill.potassium {
      background: var(--accent);
    }
    
    .trend {
      display: flex;
      align-items: center;
      gap: 5px;
      font-size: 0.9em;
      margin-top: 5px;
    }
    
    .trend.up {
      color: #4ade80;
    }
    
    .trend.down {
      color: #f87171;
    }
    
    .trend svg {
      width: 16px;
      height: 16px;
    }
    
    .sensor-grid {
      display: grid;
      grid-template-columns: repeat(2, 1fr);
      gap: 15px;
      margin-top: 20px;
    }
    
    .sensor-item {
      display: flex;
      align-items: center;
      gap: 10px;
    }
    
    .sensor-icon {
      width: 40px;
      height: 40px;
      background-color: rgba(74, 222, 128, 0.1);
      border-radius: 8px;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    
    .sensor-icon svg {
      width: 20px;
      height: 20px;
      color: var(--primary);
    }
    
    .sensor-info {
      flex: 1;
    }
    
    .sensor-title {
      font-size: 0.9em;
      color: var(--gray);
      margin-bottom: 2px;
    }
    
    .sensor-value {
      font-weight: 600;
      font-size: 1.1em;
    }
    
    .refresh-btn {
      position: absolute;
      top: 20px;
      right: 20px;
      background: rgba(255,255,255,0.1);
      border: none;
      border-radius: 8px;
      padding: 8px 12px;
      color: var(--light);
      cursor: pointer;
      display: flex;
      align-items: center;
      gap: 5px;
      transition: all 0.2s ease;
    }
    
    .refresh-btn:hover {
      background: rgba(255,255,255,0.2);
    }
    
    .refresh-btn svg {
      width: 16px;
      height: 16px;
    }
    
    @media (max-width: 768px) {
      .dashboard {
        grid-template-columns: 1fr;
      }
      
      .sensor-grid {
        grid-template-columns: 1fr;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <header>
      <h1>Soil Monitoring Dashboard</h1>
      <p>Real-time soil nutrient and environmental data</p>
    </header>
    
    <div class="dashboard">
      <div class="card">
        <h2>
          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M13 7h8m0 0v8m0-8l-8 8-4-4-6 6" />
          </svg>
          NPK Levels
        </h2>
        <div class="chart-container">
          <canvas id="npkChart"></canvas>
        </div>
        <div class="gauge-container">
          <div class="gauge-label">Nitrogen</div>
          <div class="gauge">
            <div class="gauge-fill nitrogen" id="nitrogen-gauge"></div>
          </div>
        </div>
        <div class="gauge-container">
          <div class="gauge-label">Phosphorus</div>
          <div class="gauge">
            <div class="gauge-fill phosphorus" id="phosphorus-gauge"></div>
          </div>
        </div>
        <div class="gauge-container">
          <div class="gauge-label">Potassium</div>
          <div class="gauge">
            <div class="gauge-fill potassium" id="potassium-gauge"></div>
          </div>
        </div>
      </div>
      
      <div class="card">
        <h2>
          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M12 6v6m0 0v6m0-6h6m-6 0H6" />
          </svg>
          Environmental Data
        </h2>
        <div class="sensor-grid">
          <div class="sensor-item">
            <div class="sensor-icon">
              <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M3 15a4 4 0 004 4h9a5 5 0 10-.1-9.999 5.002 5.002 0 10-9.78 2.096A4.001 4.001 0 003 15z" />
              </svg>
            </div>
            <div class="sensor-info">
              <div class="sensor-title">Temperature</div>
              <div class="sensor-value" id="temperature">--</div>
              <div class="trend" id="temp-trend">
                <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M5 10l7-7m0 0l7 7m-7-7v18" />
                </svg>
                <span>0%</span>
              </div>
            </div>
          </div>
          
          <div class="sensor-item">
            <div class="sensor-icon">
              <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M18.364 5.636l-3.536 3.536m0 5.656l3.536 3.536M9.172 9.172L5.636 5.636m3.536 9.192l-3.536 3.536M21 12a9 9 0 11-18 0 9 9 0 0118 0zm-5 0a4 4 0 11-8 0 4 4 0 018 0z" />
              </svg>
            </div>
            <div class="sensor-info">
              <div class="sensor-title">Humidity</div>
              <div class="sensor-value" id="humidity">--</div>
              <div class="trend" id="humidity-trend">
                <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
                  <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M19 14l-7 7m0 0l-7-7m7 7V3" />
                </svg>
                <span>0%</span>
              </div>
            </div>
          </div>
        </div>
      </div>
      
      <div class="card">
        <h2>
          <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M17.657 16.657L13.414 20.9a1.998 1.998 0 01-2.827 0l-4.244-4.243a8 8 0 1111.314 0z" />
            <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M15 11a3 3 0 11-6 0 3 3 0 016 0z" />
          </svg>
          Location
        </h2>
        <div id="location">Waiting for GPS signal...</div>
        <div class="map-container" id="map"></div>
      </div>
    </div>
    
    <div class="status">
      Last updated: <span id="lastUpdate">Never</span>
      <button class="refresh-btn" id="refreshBtn">
        <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke="currentColor">
          <path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="M4 4v5h.582m15.356 2A8.001 8.001 0 004.582 9m0 0H9m11 11v-5h-.581m0 0a8.003 8.003 0 01-15.357-2m15.357 2H15" />
        </svg>
        Refresh
      </button>
    </div>
  </div>

  <script>
    // Initialize NPK chart
    const npkCtx = document.getElementById('npkChart').getContext('2d');
    const npkChart = new Chart(npkCtx, {
      type: 'bar',
      data: {
        labels: ['Nitrogen', 'Phosphorus', 'Potassium'],
        datasets: [{
          label: 'NPK Levels (mg/kg)',
          data: [0, 0, 0],
          backgroundColor: [
            'rgba(74, 222, 128, 0.7)',
            'rgba(96, 165, 250, 0.7)',
            'rgba(245, 158, 11, 0.7)'
          ],
          borderColor: [
            'rgba(74, 222, 128, 1)',
            'rgba(96, 165, 250, 1)',
            'rgba(245, 158, 11, 1)'
          ],
          borderWidth: 1
        }]
      },
      options: {
        responsive: true,
        maintainAspectRatio: false,
        scales: {
          y: {
            beginAtZero: true,
            grid: {
              color: 'rgba(255, 255, 255, 0.1)'
            },
            ticks: {
              color: 'rgba(255, 255, 255, 0.7)'
            }
          },
          x: {
            grid: {
              color: 'rgba(255, 255, 255, 0.1)'
            },
            ticks: {
              color: 'rgba(255, 255, 255, 0.7)'
            }
          }
        },
        plugins: {
          legend: {
            labels: {
              color: 'rgba(255, 255, 255, 0.7)'
            }
          }
        }
      }
    });

    // Store previous values for trend calculation
    let previousValues = {
      temperature: null,
      humidity: null
    };

    function updateData() {
      fetch('/data')
        .then(response => response.json())
        .then(data => {
          // Update NPK chart
          npkChart.data.datasets[0].data = [
            data.nitrogen || 0,
            data.phosphorus || 0,
            data.potassium || 0
          ];
          npkChart.update();

          // Update gauges
          document.getElementById('nitrogen-gauge').style.width = `${Math.min(data.nitrogen || 0, 100)}%`;
          document.getElementById('phosphorus-gauge').style.width = `${Math.min(data.phosphorus || 0, 100)}%`;
          document.getElementById('potassium-gauge').style.width = `${Math.min(data.potassium || 0, 100)}%`;

          // Update environmental data with trends
          if (data.temperature !== undefined) {
            updateSensorWithTrend('temperature', data.temperature, '°C');
          }
          if (data.humidity !== undefined) {
            updateSensorWithTrend('humidity', data.humidity, '%');
          }

          // Update GPS
          if(data.latitude !== 0 && data.longitude !== 0) {
            document.getElementById('location').textContent = 
              `Latitude: ${data.latitude.toFixed(6)}, Longitude: ${data.longitude.toFixed(6)}`;
            updateMap(data.latitude, data.longitude);
          }
          
          // Update timestamp
          document.getElementById('lastUpdate').textContent = new Date().toLocaleTimeString();
        })
        .catch(error => console.error('Error fetching data:', error));
    }

    function updateSensorWithTrend(sensorId, value, unit) {
      const element = document.getElementById(sensorId);
      const trendElement = document.getElementById(`${sensorId}-trend`);
      
      // Calculate trend if we have previous value
      if (previousValues[sensorId] !== null) {
        const change = ((value - previousValues[sensorId]) / previousValues[sensorId] * 100).toFixed(1);
        const trendDirection = change >= 0 ? 'up' : 'down';
        
        trendElement.className = `trend ${trendDirection}`;
        trendElement.querySelector('span').textContent = `${Math.abs(change)}%`;
        
        // Update arrow direction
        const arrowPath = trendDirection === 'up' ? 
          'M5 10l7-7m0 0l7 7m-7-7v18' : 
          'M19 14l-7 7m0 0l-7-7m7 7V3';
        trendElement.querySelector('svg').innerHTML = `<path stroke-linecap="round" stroke-linejoin="round" stroke-width="2" d="${arrowPath}" />`;
      }
      
      // Update current value
      element.textContent = `${value} ${unit}`;
      previousValues[sensorId] = parseFloat(value);
    }

    function updateMap(lat, lng) {
      const map = document.getElementById('map');
      map.innerHTML = `<iframe width="100%" height="100%" frameborder="0" style="border:0" 
        src="https://maps.google.com/maps?q=${lat},${lng}&z=15&output=embed&hl=en&t=m&theme=dark" allowfullscreen></iframe>`;
    }

    // Manual refresh button
    document.getElementById('refreshBtn').addEventListener('click', updateData);

    // Update data immediately and then every 2 seconds (as in original)
    updateData();
    setInterval(updateData, 2000);

    // Add animation to cards on load
    document.addEventListener('DOMContentLoaded', () => {
      const cards = document.querySelectorAll('.card');
      cards.forEach((card, index) => {
        setTimeout(() => {
          card.style.opacity = '1';
          card.style.transform = 'translateY(0)';
        }, index * 100);
      });
    });
  </script>
</body>
</html>
  )=====";