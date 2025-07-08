// components/ParameterCard.js - 基于您示例的简化版本

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
            <span className="info-container">
              <span className="info-icon">ℹ️</span>
              <span className="tooltip">{tooltipText}</span>
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