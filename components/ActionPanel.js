// components/ActionPanel.js - 操作面板组件

const ActionPanel = ({ loading, onRefresh }) => {
  return (
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
        
        <button className="action-button secondary">
          📊 View Historical Data
        </button>
        
        <button className="action-button secondary">
          📋 Generate Report
        </button>
        
        <button className="action-button secondary">
          ⚙️ Sensor Settings
        </button>
      </div>
    </div>
  )
}

export default ActionPanel