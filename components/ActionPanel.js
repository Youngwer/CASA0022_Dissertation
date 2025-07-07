// components/ActionPanel.js - 简化的操作面板组件

import { useState } from 'react'
import HistoryModal from './HistoryModal'

const ActionPanel = ({ loading, onRefresh }) => {
  const [showHistory, setShowHistory] = useState(false)

  const handleViewHistory = () => {
    setShowHistory(true)
  }

  const handleCloseHistory = () => {
    setShowHistory(false)
  }

  return (
    <>
      <div className="action-panel">
        <h3>Quick Actions</h3>
        <div className="action-buttons">
          <button 
            onClick={onRefresh} 
            className="action-button primary"
            disabled={loading}
          >
            {loading ? '🔄 Loading...' : '🔄 Refresh Data'}
          </button>
          
          <button 
            onClick={handleViewHistory}
            className="action-button secondary"
          >
            📊 View Historical Data
          </button>
        </div>
      </div>

      {/* 历史记录弹窗 */}
      <HistoryModal 
        isOpen={showHistory} 
        onClose={handleCloseHistory} 
      />
    </>
  )
}

export default ActionPanel