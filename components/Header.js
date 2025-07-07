// components/Header.js - 头部导航组件（带快捷操作）

import { useState } from 'react'
import { formatTime, formatDate, getDataSourceText } from '../utils/waterQualityUtils'
import HistoryModal from './HistoryModal'

const Header = ({ 
  currentTime, 
  mounted, 
  isOnline, 
  loading, 
  dataSource,
  onRefresh // 添加刷新功能回调
}) => {
  const [showHistory, setShowHistory] = useState(false)

  const handleRefresh = () => {
    if (onRefresh && !loading) {
      onRefresh()
    }
  }

  const handleViewHistory = () => {
    setShowHistory(true)
  }

  const handleCloseHistory = () => {
    setShowHistory(false)
  }

  return (
    <>
      <header className="header">
        <div className="container">
          <div className="header-content">
            <div className="logo">
              <h1>💧 AquaMonitor</h1>
              <span className="subtitle">Water Quality System</span>
            </div>
            
            {/* 中间区域 - 快捷操作 */}
            <div className="header-center">
              <button 
                onClick={handleRefresh}
                className={`header-action-btn refresh-btn ${loading ? 'loading' : ''}`}
                disabled={loading}
                title="Refresh Data"
              >
                {loading ? '🔄' : '🔄'} Refresh
              </button>
              
              <button 
                onClick={handleViewHistory}
                className="header-action-btn history-btn"
                title="View History"
              >
                📊 History
              </button>
            </div>

            {/* 右侧时间和状态 */}
            <div className="header-right">
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
            </div>
          </div>
        </div>
      </header>

      {/* 历史记录弹窗 */}
      <HistoryModal 
        isOpen={showHistory} 
        onClose={handleCloseHistory} 
      />
    </>
  )
}

export default Header