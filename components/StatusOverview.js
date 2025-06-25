// components/StatusOverview.js - 水质状态总览组件

import { 
  getStatusColor, 
  getStatusText, 
  getStatusDescription, 
  getDataSourceText 
} from '../utils/waterQualityUtils'

const StatusOverview = ({ 
  waterData, 
  dataSource 
}) => {
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