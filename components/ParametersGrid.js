// components/ParametersGrid.js - 改进的参数卡片网格组件

import ParameterCard from './ParameterCard'

const ParametersGrid = ({ waterData }) => {
  // 四个核心水质参数，移除了中文描述，只在悬浮框中显示英文说明
  const coreParameters = [
    {
      icon: '⚗️',
      title: 'pH Level',
      value: waterData.ph,
      unit: '',
      param: 'ph',
      decimals: 1
    },
    {
      icon: '🌫️',
      title: 'Turbidity',
      value: waterData.turbidity,
      unit: 'NTU',
      param: 'turbidity',
      decimals: 1
    },
    {
      icon: '⚡',
      title: 'Conductivity',
      value: waterData.conductivity,
      unit: 'μS/cm',
      param: 'conductivity',
      decimals: 1
    },
    {
      icon: '💎',
      title: 'TDS',
      value: waterData.tds,
      unit: 'ppm',
      param: 'tds',
      decimals: 1
    }
  ]

  return (
    <div className="parameters-container">
      {/* 四个核心参数一行显示 */}
      <div className="core-params-row">
        {coreParameters.map((parameter) => (
          <ParameterCard
            key={parameter.param}
            icon={parameter.icon}
            title={parameter.title}
            value={parameter.value}
            unit={parameter.unit}
            param={parameter.param}
            decimals={parameter.decimals}
          />
        ))}
      </div>
    </div>
  )
}

export default ParametersGrid