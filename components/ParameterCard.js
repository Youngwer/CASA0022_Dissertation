// components/ParameterCard.js - 单个参数卡片组件

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
  decimals = 1 
}) => {
  const status = getParameterStatus(param, value)
  const description = getParameterDescription(param, value, status)
  const reference = getParameterReference(param)

  return (
    <div className={`parameter-card ${status}`}>
      <div className="card-icon">{icon}</div>
      <div className="card-content">
        <h3>{title}</h3>
        <div className="value">
          <span className="number">{value.toFixed(decimals)}</span>
          <span className="unit">{unit}</span>
        </div>
        <div className="description">
          {description}
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