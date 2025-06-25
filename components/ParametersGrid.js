// components/ParametersGrid.js - 参数卡片网格组件

import ParameterCard from './ParameterCard'

const ParametersGrid = ({ waterData }) => {
  const parameters = [
    {
      icon: '🌡️',
      title: 'Water Temperature',
      value: waterData.temperature,
      unit: '°C',
      param: 'temperature',
      decimals: 1
    },
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
      title: 'TDS (Total Dissolved Solids)',
      value: waterData.tds,
      unit: 'ppm',
      param: 'tds',
      decimals: 1
    }
  ]

  return (
    <div className="parameters-grid">
      {parameters.map((parameter, index) => (
        <ParameterCard
          key={index}
          icon={parameter.icon}
          title={parameter.title}
          value={parameter.value}
          unit={parameter.unit}
          param={parameter.param}
          decimals={parameter.decimals}
        />
      ))}
    </div>
  )
}

export default ParametersGrid