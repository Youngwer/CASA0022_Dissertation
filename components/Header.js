// components/Header.js - 头部导航组件

import { formatTime, formatDate, getDataSourceText } from '../utils/waterQualityUtils'

const Header = ({ 
  currentTime, 
  mounted, 
  isOnline, 
  loading, 
  dataSource 
}) => {
  return (
    <header className="header">
      <div className="container">
        <div className="header-content">
          <div className="logo">
            <h1>💧 AquaMonitor</h1>
            <span className="subtitle">Water Quality System</span>
          </div>
          
          <div className="header-info">
            <div className="time-display">
              <div className="current-time">
                {formatTime(currentTime, mounted)}
              </div>
              <div className="current-date">
                {formatDate(currentTime, mounted)}
              </div>
            </div>
            
            <div className="status-indicator">
              <div className={`status-dot ${isOnline ? 'online' : 'offline'}`}></div>
              <span>{isOnline ? 'Online' : 'Offline'}</span>
              {loading && <span className="loading-text"> • Loading...</span>}
            </div>
            
            {dataSource && (
              <div className="data-source">
                <span className="source-label">Source:</span>
                <span className="source-value">{getDataSourceText(dataSource)}</span>
              </div>
            )}
          </div>
        </div>
      </div>
    </header>
  )
}

export default Header