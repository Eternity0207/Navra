import React from 'react';
import { Cpu, Zap, CheckCircle } from 'lucide-react';

const AlgorithmCard = ({ algorithm }) => {
  return (
    <div className="algorithm-card">
      <div className="algorithm-header">
        <Cpu size={28} />
        <h3>{algorithm.name}</h3>
      </div>
      
      <p className="algorithm-description">{algorithm.description}</p>
      
      <div className="algorithm-details">
        <div className="detail-item">
          <Zap size={18} />
          <div>
            <span className="detail-label">Complexity:</span>
            <span className="detail-value">{algorithm.complexity}</span>
          </div>
        </div>
        
        <div className="detail-item">
          <CheckCircle size={18} />
          <div>
            <span className="detail-label">Use Case:</span>
            <span className="detail-value">{algorithm.useCase}</span>
          </div>
        </div>
      </div>

      <div className="algorithm-features">
        <h4>Features:</h4>
        <ul>
          {algorithm.features.map((feature, index) => (
            <li key={index}>{feature}</li>
          ))}
        </ul>
      </div>
    </div>
  );
};

export default AlgorithmCard;
