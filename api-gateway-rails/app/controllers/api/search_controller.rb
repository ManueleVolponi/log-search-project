# api-gateway-rails/app/controllers/api/search_controller.rb

module Api
  class SearchController < ApplicationController
    
    def index
      query = params[:q]
      
      if query.blank?
        render json: { error: "Missing required query parameter 'q'" }, status: :bad_request
        return
      end

      begin
        results = CppSearchService.call(query)
        
        render json: results
        
      rescue StandardError => e
        Rails.logger.error("API Gateway error: #{e.message}")
        render json: { error: "Internal search error: #{e.message}" }, status: :internal_server_error
      end
    end
    
  end
end