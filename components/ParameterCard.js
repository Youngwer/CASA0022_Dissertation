// components/ParameterCard.js - 简化的参数卡片组件

import { useState } from 'react'
import { 
  getParameterStatus, 
  getParameterDescription, 
  getParameterReference,
  getParameterTooltip
} from '../utils/waterQualityUtils'

const ParameterCard = ({ 
  icon, 
  title, 
  value, 
  unit, 
  param, 
  decimals = 1
}) => {
  const [showTooltip, setShowTooltip] = useState(false)
  
  const status = getParameterStatus(param, value)
  const statusDescription = getParameterDescription(param, value, status)
  const reference = getParameterReference(param)
  const tooltipText = getParameterTooltip(param)

  return (
    <div className={`parameter-card ${status}`}>
      <div className="card-icon">{icon}</div>
      <div className="card-content">
        <div className="card-title-section">
          <h3>
            {title}
            <span 
              className="info-icon"
              onMouseEnter={() => setShowTooltip(true)}
              onMouseLeave={() => setShowTooltip(false)}
            >
              ℹ️
              {showTooltip && (
                <div className="tooltip">
                  <div className="tooltip-content">
                    <p className="tooltip-description">
                      {tooltipText}
                    </p>
                  </div>
                </div>
              )}
            </span>
          </h3>
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