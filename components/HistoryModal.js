// components/HistoryModal.js - 带水质标签和电导率的历史记录弹窗组件

import { useState, useEffect } from 'react'
import { getStatusColor, getStatusText } from '../utils/waterQualityUtils'

const HistoryModal = ({ isOpen, onClose }) => {
  const [loading, setLoading] = useState(false)
  const [records, setRecords] = useState([])
  const [error, setError] = useState(null)

  // 获取历史数据
  const fetchHistoryData = async () => {
    try {
      setLoading(true)
      setError(null)

      const response = await fetch('/api/recent-history?limit=20')
      const result = await response.json()

      if (result.success) {
        setRecords(result.data || [])
      } else {
        throw new Error(result.message || 'Failed to load history')
      }
    } catch (err) {
      console.error('Error loading history:', err)
      setError(err.message)
    } finally {
      setLoading(false)
    }
  }

  // 当弹窗打开时获取数据
  useEffect(() => {
    if (isOpen) {
      fetchHistoryData()
    }
  }, [isOpen])

  // 格式化时间
  const formatDateTime = (dateString) => {
    return new Date(dateString).toLocaleString('en-GB', {
      month: 'short',
      day: 'numeric',
      hour: '2-digit',
      minute: '2-digit'
    })
  }

  // 获取时间间隔
  const getTimeAgo = (dateString) => {
    const now = new Date()
    const past = new Date(dateString)
    const diffHours = Math.round((now - past) / (1000 * 60 * 60))
    
    if (diffHours < 1) return 'Now'
    if (diffHours < 24) return `${diffHours}h ago`
    const diffDays = Math.round(diffHours / 24)
    return `${diffDays}d ago`
  }

  // 获取水质标签显示
  const getWaterLabel = (record) => {
    if (record.water_label) {
      return record.water_label
    }
    // 如果没有标签，显示默认文本
    return 'Unlabeled Sample'
  }

  if (!isOpen) return null

  return (
    <div className="modal-overlay" onClick={onClose}>
      <div className="history-modal" onClick={(e) => e.stopPropagation()}>
        {/* 弹窗头部 */}
        <div className="modal-header">
          <h3>📊 Recent Measurements</h3>
          <button onClick={onClose} className="close-button">✕</button>
        </div>

        {/* 弹窗内容 */}
        <div className="modal-content">
          {loading && (
            <div className="modal-loading">
              <div className="loading-spinner">🔄</div>
              <p>Loading records...</p>
            </div>
          )}

          {error && (
            <div className="modal-error">
              <span>❌ {error}</span>
              <button onClick={fetchHistoryData} className="retry-btn">🔄 Retry</button>
            </div>
          )}

          {!loading && !error && records.length === 0 && (
            <div className="modal-empty">
              <p>📭 No measurement records found</p>
            </div>
          )}

          {!loading && !error && records.length > 0 && (
            <div className="history-table-container">
              <table className="history-table">
                <thead>
                  <tr>
                    <th>Time</th>
                    <th>Water Source</th>
                    <th>pH</th>
                    <th>Temp</th>
                    <th>Conductivity</th>
                    <th>TDS</th>
                    <th>Turbidity</th>
                    <th>Status</th>
                  </tr>
                </thead>
                <tbody>
                  {records.map((record) => (
                    <tr key={record.id}>
                      <td className="time-cell">
                        <div className="time-main">{formatDateTime(record.recorded_at)}</div>
                        <div className="time-ago">{getTimeAgo(record.recorded_at)}</div>
                      </td>
                      <td className="water-label-cell">
                        <div className="water-label-display">
                          <span className="label-icon">🏷️</span>
                          <span className="label-text">{getWaterLabel(record)}</span>
                        </div>
                      </td>
                      <td>{record.ph?.toFixed(1) || 'N/A'}</td>
                      <td>{record.temperature?.toFixed(1) || 'N/A'}°C</td>
                      <td>{record.conductivity?.toFixed(0) || 'N/A'} μS/cm</td>
                      <td>{record.tds?.toFixed(0) || 'N/A'} ppm</td>
                      <td>{record.turbidity?.toFixed(1) || 'N/A'} NTU</td>
                      <td>
                        <span 
                          className="status-badge" 
                          style={{ 
                            backgroundColor: getStatusColor(record.status),
                            color: 'white'
                          }}
                        >
                          {getStatusText(record.status)}
                        </span>
                      </td>
                    </tr>
                  ))}
                </tbody>
              </table>
            </div>
          )}
        </div>

        {/* 弹窗底部 */}
        <div className="modal-footer">
          <div className="record-count">
            Showing {records.length} recent measurements
          </div>
        </div>
      </div>
    </div>
  )
}

export default HistoryModal