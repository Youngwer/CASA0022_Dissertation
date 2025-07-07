// components/StatusOverview.js - 带温度和水质标签的状态总览组件

import { useState } from 'react'
import { 
  getStatusColor, 
  getStatusText, 
  getStatusDescription, 
  getDataSourceText 
} from '../utils/waterQualityUtils'
import WaterLabelInput from './WaterLabelInput'

const StatusOverview = ({ 
  waterData, 
  dataSource,
  onDataRefresh // 用于刷新数据的回调
}) => {
  const [currentLabel, setCurrentLabel] = useState(waterData.waterLabel || '')

  // 处理标签更新
  const handleLabelUpdate = (newLabel) => {
    setCurrentLabel(newLabel)
    // 如果需要，可以触发数据刷新
    if (onDataRefresh) {
      onDataRefresh()
    }
  }

  return (
    <div className="overview-card">
      <div className="card-header">
        <h2>Real-time Water Quality Status</h2>
        <div className="last-update">
          <div>Last Update: {waterData.lastUpdate}</div>
        </div>
      </div>

      {/* 主要内容区域 - 左右布局 */}
      <div className="overview-main-content">
        {/* 左侧 - 水质标签输入区域 */}
        <div className="water-label-section">
          <WaterLabelInput 
            currentLabel={currentLabel}
            onLabelUpdate={handleLabelUpdate}
          />
        </div>
        
        {/* 右侧 - 水质状态评估 */}
        <div 
          className="status-banner" 
          style={{backgroundColor: getStatusColor(waterData.status)}}
        >
          <div className="status-main-content">
            <div className="status-content">
              <span className="status-label">Water Quality Assessment</span>
              <span className="status-value">{getStatusText(waterData.status)}</span>
            </div>
            
            {/* 简化的温度显示 */}
            <div className="temperature-display">
              <span className="temperature-icon">🌡️</span>
              <span className="temperature-value">
                {waterData.temperature.toFixed(1)}°C
              </span>
            </div>
          </div>
          
          <div className="status-description">
            {getStatusDescription(waterData.status)}
          </div>
        </div>
      </div>
    </div>
  )
}

export default StatusOverview