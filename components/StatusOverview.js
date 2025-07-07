// components/StatusOverview.js - 带水质标签的状态总览组件

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
  onDataRefresh // 新增：用于刷新数据的回调
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
          <div style={{fontSize: '0.8rem', opacity: 0.7}}>
            Source: {getDataSourceText(dataSource)}
          </div>
        </div>
      </div>

      {/* 水质标签输入区域 */}
      <div className="water-label-section">
        <WaterLabelInput 
          currentLabel={currentLabel}
          onLabelUpdate={handleLabelUpdate}
        />
      </div>
      
      <div 
        className="status-banner" 
        style={{backgroundColor: getStatusColor(waterData.status)}}
      >
        <div className="status-content">
          <span className="status-label">Water Quality Assessment</span>
          <span className="status-value">{getStatusText(waterData.status)}</span>
        </div>
        <div style={{
          fontSize: '0.9rem', 
          marginTop: '0.5rem', 
          opacity: 0.9,
          textAlign: 'center'
        }}>
          {getStatusDescription(waterData.status)}
        </div>
      </div>
    </div>
  )
}

export default StatusOverview