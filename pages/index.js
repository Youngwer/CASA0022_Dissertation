// pages/index.js - Water Quality Dashboard (Hydration Error Fixed)
import Head from 'next/head'
import { useState, useEffect } from 'react'

export default function Home() {
  const [currentTime, setCurrentTime] = useState(null) // 初始为null避免hydration错误
  const [isOnline, setIsOnline] = useState(false)
  const [loading, setLoading] = useState(true)
  const [error, setError] = useState(null)
  const [mounted, setMounted] = useState(false) // 添加mounted状态
  const [waterData, setWaterData] = useState({
    temperature: 0,
    ph: 0,
    turbidity: 0,
    conductivity: 0,
    tds: 0,
    status: 'UNKNOWN',
    lastUpdate: 'Loading...'
  })
  const [dataSource, setDataSource] = useState('')

  // 获取真实传感器数据
  const fetchWaterData = async () => {
    try {
      setLoading(true)
      setError(null)
      
      console.log('🚀 Fetching water quality data...')
      
      const response = await fetch('/api/water-data')
      const result = await response.json()

      console.log('📦 API Response:', result)

      if (result.success && result.data) {
        setWaterData({
          temperature: result.data.temperature || 0,
          ph: result.data.ph || 0,
          turbidity: result.data.turbidity || 0,
          conductivity: result.data.conductivity || 0,
          tds: result.data.tds || 0,
          status: result.data.status || 'UNKNOWN',
          lastUpdate: result.data.lastUpdate || 'Unknown'
        })
        setDataSource(result.source)
        setIsOnline(true)
        console.log('✅ Data loaded successfully')
      } else {
        throw new Error(result.message || 'Failed to load data')
      }
    } catch (err) {
      console.error('❌ Error loading data:', err)
      setError(`Failed to load data: ${err.message}`)
      setIsOnline(false)
    } finally {
      setLoading(false)
    }
  }

  // 只在客户端运行时间更新和数据获取
  useEffect(() => {
    // 设置mounted状态，避免hydration错误
    setMounted(true)
    setCurrentTime(new Date())
    
    const timer = setInterval(() => {
      setCurrentTime(new Date())
    }, 1000)
    
    // 初始加载数据
    fetchWaterData()
    
    // 每30秒自动刷新数据
    const dataTimer = setInterval(fetchWaterData, 30000)
    
    return () => {
      clearInterval(timer)
      clearInterval(dataTimer)
    }
  }, [])

  // Get water quality status color
  const getStatusColor = (status) => {
    switch(status) {
      case 'GOOD': return '#10B981' // Green
      case 'OK': return '#F59E0B'   // Yellow
      case 'POOR': return '#EF4444' // Red
      default: return '#6B7280'     // Gray
    }
  }

  // Get water quality status text
  const getStatusText = (status) => {
    switch(status) {
      case 'GOOD': return 'Excellent'
      case 'OK': return 'Good'
      case 'POOR': return 'Needs Improvement'
      default: return 'Unknown'
    }
  }

  // Get data source display text
  const getDataSourceText = (source) => {
    const sourceConfig = {
      'ttn_storage': '🌐 TTN Message Storage (Real-time)',
      'ttn_uplink': '📡 TTN Live Data Stream',
      'database': '💾 Local Database',
      'database_fallback': '💾 Database Backup',
      'mock': '🎭 Demo Data'
    }
    return sourceConfig[source] || source
  }

  // Get parameter status (for individual cards)
  const getParameterStatus = (param, value) => {
    switch(param) {
      case 'ph':
        if (value >= 6.5 && value <= 8.5) return 'good'
        if (value >= 6.0 && value <= 9.0) return 'ok'
        return 'warning'
      case 'turbidity':
        if (value < 1) return 'good'
        if (value < 4) return 'ok'
        return 'warning'
      case 'tds':
        if (value < 50) return 'good'
        if (value < 300) return 'ok'
        return 'warning'
      case 'temperature':
        if (value >= 5 && value <= 25) return 'good'
        if (value >= 0 && value <= 35) return 'ok'
        return 'warning'
      default:
        return 'good'
    }
  }

  // 防止hydration错误的时间显示函数
  const formatTime = () => {
    if (!mounted || !currentTime) return '--:--:--'
    return currentTime.toLocaleTimeString()
  }

  const formatDate = () => {
    if (!mounted || !currentTime) return 'Loading...'
    return currentTime.toLocaleDateString()
  }

  return (
    <div className="dashboard">
      <Head>
        <title>Water Quality Dashboard</title>
        <meta name="description" content="Real-time Water Quality Monitoring & Analysis System" />
        <meta name="viewport" content="width=device-width, initial-scale=1" />
        <link rel="icon" href="/favicon.ico" />
      </Head>

      {/* Header Navigation */}
      <header className="header">
        <div className="container">
          <div className="header-content">
            <div className="logo">
              <h1>💧 AquaMonitor</h1>
              <span className="subtitle">Water Quality System</span>
            </div>
            <div className="status-indicator">
              <div className={`status-dot ${isOnline ? 'online' : 'offline'}`}></div>
              <span>{isOnline ? 'Online' : 'Offline'}</span>
              {loading && <span className="loading-text"> • Loading...</span>}
            </div>
          </div>
        </div>
      </header>

      {/* Main Content Area */}
      <main className="main">
        <div className="container">
          
          {/* Error Message */}
          {error && (
            <div className="error-banner">
              <p>⚠️ {error}</p>
              <button onClick={fetchWaterData} className="retry-button">
                🔄 Retry
              </button>
            </div>
          )}
          
          {/* Overview Card */}
          <div className="overview-card">
            <div className="card-header">
              <h2>Real-time Water Quality Status</h2>
              <div className="last-update">
                <div>Last Update: {waterData.lastUpdate}</div>
                <div style={{fontSize: '0.8rem', opacity: 0.7}}>
                  Source: {getDataSourceText(dataSource)}
                </div>
              </div>
            </div>
            
            <div className="status-banner" style={{backgroundColor: getStatusColor(waterData.status)}}>
              <div className="status-content">
                <span className="status-label">Water Quality</span>
                <span className="status-value">{getStatusText(waterData.status)}</span>
              </div>
            </div>
          </div>

          {/* Parameter Cards Grid */}
          <div className="parameters-grid">
            
            {/* Temperature Card */}
            <div className={`parameter-card ${getParameterStatus('temperature', waterData.temperature) === 'warning' ? 'warning' : ''}`}>
              <div className="card-icon">🌡️</div>
              <div className="card-content">
                <h3>Water Temperature</h3>
                <div className="value">
                  <span className="number">{waterData.temperature.toFixed(2)}</span>
                  <span className="unit">°C</span>
                </div>
                <div className="description">
                  {getParameterStatus('temperature', waterData.temperature) === 'good' ? 'Optimal temperature' : 
                   getParameterStatus('temperature', waterData.temperature) === 'ok' ? 'Acceptable range' : 'Outside optimal range'}
                </div>
              </div>
            </div>

            {/* pH Value Card */}
            <div className={`parameter-card ${getParameterStatus('ph', waterData.ph) === 'warning' ? 'warning' : ''}`}>
              <div className="card-icon">⚗️</div>
              <div className="card-content">
                <h3>pH Level</h3>
                <div className="value">
                  <span className="number">{waterData.ph.toFixed(2)}</span>
                  <span className="unit"></span>
                </div>
                <div className="description">
                  {getParameterStatus('ph', waterData.ph) === 'good' ? 'Ideal pH balance' : 
                   getParameterStatus('ph', waterData.ph) === 'ok' ? 'Acceptable pH level' : 'pH needs attention'}
                </div>
              </div>
            </div>

            {/* Turbidity Card */}
            <div className={`parameter-card ${getParameterStatus('turbidity', waterData.turbidity) === 'warning' ? 'warning' : ''}`}>
              <div className="card-icon">🌫️</div>
              <div className="card-content">
                <h3>Turbidity</h3>
                <div className="value">
                  <span className="number">{waterData.turbidity.toFixed(1)}</span>
                  <span className="unit">NTU</span>
                </div>
                <div className="description">
                  {getParameterStatus('turbidity', waterData.turbidity) === 'good' ? 'Crystal clear water' : 
                   getParameterStatus('turbidity', waterData.turbidity) === 'ok' ? 'Good clarity' : 'High turbidity detected'}
                </div>
              </div>
            </div>

            {/* Conductivity Card */}
            <div className="parameter-card">
              <div className="card-icon">⚡</div>
              <div className="card-content">
                <h3>Conductivity</h3>
                <div className="value">
                  <span className="number">{waterData.conductivity}</span>
                  <span className="unit">μS/cm</span>
                </div>
                <div className="description">Dissolved substances content</div>
              </div>
            </div>

            {/* TDS Card */}
            <div className={`parameter-card ${getParameterStatus('tds', waterData.tds) === 'warning' ? 'warning' : ''}`}>
              <div className="card-icon">💎</div>
              <div className="card-content">
                <h3>TDS (Total Dissolved Solids)</h3>
                <div className="value">
                  <span className="number">{waterData.tds}</span>
                  <span className="unit">ppm</span>
                </div>
                <div className="description">
                  {getParameterStatus('tds', waterData.tds) === 'good' ? 'Excellent purity' : 
                   getParameterStatus('tds', waterData.tds) === 'ok' ? 'Good purity level' : 'High dissolved solids'}
                </div>
              </div>
            </div>

            {/* Time Card - 使用防hydration错误的方法 */}
            <div className="parameter-card time-card">
              <div className="card-icon">🕐</div>
              <div className="card-content">
                <h3>Current Time</h3>
                <div className="value">
                  <span className="time">{formatTime()}</span>
                </div>
                <div className="description">{formatDate()}</div>
              </div>
            </div>
          </div>

          {/* Educational Information Section */}
          <div className="education-section">
            <h2>💡 Water Quality Parameters Guide</h2>
            <div className="education-grid">
              <div className="education-card">
                <h4>🌡️ Temperature Standards</h4>
                <p><strong>Ideal Range:</strong> 5-25°C</p>
                <p>Temperature affects taste and bacterial growth rate</p>
              </div>
              
              <div className="education-card">
                <h4>⚗️ pH Standards</h4>
                <p><strong>Drinking Water:</strong> 6.5-8.5</p>
                <p>pH indicates acid-base balance</p>
              </div>
              
              <div className="education-card">
                <h4>🌫️ Turbidity Standards</h4>
                <p><strong>Excellent:</strong> &lt;1 NTU</p>
                <p><strong>Acceptable:</strong> &lt;4 NTU</p>
                <p>Lower turbidity means clearer water</p>
              </div>
              
              <div className="education-card">
                <h4>💎 TDS Standards</h4>
                <p><strong>Excellent:</strong> 0-50 ppm</p>
                <p><strong>Good:</strong> 50-150 ppm</p>
                <p><strong>Acceptable:</strong> 150-300 ppm</p>
              </div>
            </div>
          </div>

          {/* Action Buttons Section */}
          <div className="action-section">
            <button className="action-button primary" onClick={fetchWaterData} disabled={loading}>
              {loading ? '🔄 Loading...' : '🔄 Refresh Data'}
            </button>
            <button className="action-button secondary">
              📊 View Historical Data
            </button>
            <button className="action-button secondary">
              📋 Generate Report
            </button>
          </div>

        </div>
      </main>

      {/* Footer */}
      <footer className="footer">
        <div className="container">
          <p>&copy; 2024 AquaMonitor - Making Every Drop Trustworthy</p>
          <p>Powered by IoT & LoRaWAN Technology • Real-time Sensor Data</p>
        </div>
      </footer>
    </div>
  )
}