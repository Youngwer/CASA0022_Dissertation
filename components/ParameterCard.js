// components/ParameterCard.js - 单个参数卡片组件（带信息图标）

import { useState } from 'react'
import { 
  getParameterStatus, 
  getParameterDescription, 
  getParameterReference 
} from '../utils/waterQualityUtils'

const ParameterCard = ({ 
  icon, 
  title, 
  value, 
  unit, 
  param, 
  decimals = 1,
  description 
}) => {
  const [showTooltip, setShowTooltip] = useState(false)
  const status = getParameterStatus(param, value)
  const statusDescription = getParameterDescription(param, value, status)
  const reference = getParameterReference(param)

  return (
    <div className={`parameter-card ${status}`}>
      <div className="card-icon">{icon}</div>
      <div className="card-content">
        <div className="card-title-section">
          <h3>{title}</h3>
          <div 
            className="info-icon"
            onMouseEnter={() => setShowTooltip(true)}
            onMouseLeave={() => setShowTooltip(false)}
          >
            ℹ️
            {showTooltip && (
              <div className="tooltip">
                <div className="tooltip-content">
                  <p className="tooltip-description">{description}</p>
                  {reference && (
                    <div className="tooltip-reference">
                      <strong>参考范围:</strong><br />
                      {reference}
                    </div>
                  )}
                </div>
              </div>
            )}
          </div>
        </div>
        
        <div className="value">
          <span className="number">{value.toFixed(decimals)}</span>
          <span className="unit">{unit}</span>
        </div>
        
        <div className="description">
          {statusDescription}
        </div>
        
        {reference && (
          <div className="reference">
            {reference}
          </div>
        )}
      </div>
    </div>
  )
}

export default ParameterCard